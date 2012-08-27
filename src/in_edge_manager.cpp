#include "in_edge_manager.hpp"
#include "bus.hpp"
#include "kibitz_util.hpp"
#include "context.hpp"
#include "worker_broadcast_message.hpp"
#include "worker_map.hpp"

namespace kibitz {
  in_edge_manager::in_edge_manager( context& ctx  ) 
    :context_( ctx )   {
  }

  in_edge_manager::~in_edge_manager()   {
  }

  void in_edge_manager::create_bindings( zmq_pollitem_t** pollitems, int& count_items, int& size_items ) {
    release_bindings( *pollitems, count_items );
    worker_infos_t all_infos; 
    worker_map_ptr_t worker_map_ptr = worker_map::get_worker_map( context_.zmq_context() );
    worker_types_t worker_types = context_.get_worker_types();
    BOOST_FOREACH( const string& worker_type, worker_types ) {
      DLOG(INFO) << "Resolving workers for " << worker_type ;
      worker_infos_t worker_infos = worker_map_ptr->get_in_edge_workers( worker_type );
      DLOG(INFO) << "Got " << worker_infos.size() << " hits for " << worker_type;
      all_infos.insert( all_infos.end(), worker_infos.begin(), worker_infos.end() );
    }

    DLOG(INFO) << "Total in edges = " << all_infos.size() ;
    if( size_items < (all_infos.size() + 1) ) {
      size_items = all_infos.size() + 1;
      *pollitems = (zmq_pollitem_t*)realloc( *pollitems, size_items * sizeof(zmq_pollitem_t) );
    }

    // create bindings for all in edges, in edges
    // start at index 1, index 0 is broadcast notifications
    count_items = all_infos.size() + 1;
    int current = 1;
    BOOST_FOREACH( const worker_info& info, all_infos ) {
      create_binding( info, (*pollitems)[current] ); 
      current++;
    }

  }

  void in_edge_manager::create_binding( const worker_info& info, zmq_pollitem_t& pollitem ) {

    stringstream stm;
    stm << "tcp://" << info.host << ":" << info.port;
    pollitem.socket = util::create_socket( context_.zmq_context(), ZMQ_SUB );
    pollitem.fd = 0;
    pollitem.events = ZMQ_POLLIN;
    pollitem.revents = 0;
    util::check_zmq( zmq_connect( pollitem.socket, stm.str().c_str() ) );
    util::check_zmq( zmq_setsockopt( pollitem.socket, ZMQ_SUBSCRIBE, "", 0 ) );
    
  }

  void in_edge_manager::release_bindings( zmq_pollitem_t* pollitems, int count_items ) {
    // don't close item zero socket, its managed by broadcast_subscriber
    for( int item = 1; item < count_items; ++item ) {
      util::close_socket( pollitems[item].socket );
    }
  }

  void in_edge_manager::handle_notification_message( zmq_pollitem_t** pollitems, int& count_items, int& size_items ) {
    zmq_pollitem_t* items = *pollitems;
  
    if( (items[0].revents | ZMQ_POLLIN) == ZMQ_POLLIN ) {
      string json;
      util::recv( items[0].socket, json );
      DLOG(INFO) << "in edge manger got " << json ;
      notification_message_ptr_t notification_message_ptr = dynamic_pointer_cast<notification_message>( message_factory( json ) );
      string notification_type = notification_message_ptr->notification_type() ;
      if( notification_type == "worker_broadcast" ) {
	worker_broadcast_message_ptr_t broadcast_ptr = dynamic_pointer_cast<worker_broadcast_message>( notification_message_ptr);
	string notification = broadcast_ptr->notification();
	// create subscriptions for in edges
	if( notification == "create_bindings" ) {
	  DLOG(INFO) << "creating bindings";
	  create_bindings( pollitems, count_items, size_items );
	} 
      } else {
	LOG(WARNING) << "in edge manager get a message that it doesn't understand - " << json ;
      }
    }

  }

  void in_edge_manager::handle_collaboration_message( collaboration_context_t& context ) {
    for( int item = 1; item < context.count_items; ++item ) {
      if( ( context.pollitems[item].revents | ZMQ_POLLIN ) == ZMQ_POLLIN ) {
	string json ;
	util::recv( context.pollitems[item].socket, json );
	LOG(INFO) << "Got collaboration message -> " << json ;
	
      }
    }
  }

  void in_edge_manager::operator()() {
    DLOG(INFO) << "in edge manager thread started" ;
    zmq_pollitem_t* pollitems = NULL;
    int count_items = 0;
    int size_items = 0;

    try {
      // TODO: fix this if broadcast subscriber is created before broadcast publisher
      //       an exception errno 111 is raised, thus this sleep hack
      //       we want to give the publisher time to be instantiated
      sleep( 3 );
      sub broadcast_subscriber( context_.zmq_context(), HEARTBEAT_RECEIVER_BROADCASTS );
      boost::thread*  edge_monitor_thread = NULL;
      zmq_pollitem_t broadcast_pollitem = {
	broadcast_subscriber.socket(),
	0,
	ZMQ_POLLIN,
	0
      };

      pollitems = (zmq_pollitem_t*)malloc( sizeof( zmq_pollitem_t ) );
      count_items = size_items = 1;
      pollitems[0].socket = broadcast_subscriber.socket();
      pollitems[0].fd = 0;
      pollitems[0].events = ZMQ_POLLIN;
      pollitems[0].revents = 0;

      while( true ) {
	int rc = zmq_poll( pollitems, count_items, -1 ); 
	if( rc > 0 ) {
	  handle_notification_message( &pollitems, count_items, size_items );
	  //handle_collaboration_messages( 
	}

      }
    } catch( const util::queue_interrupt& ) {
      DLOG(INFO) << "interrupt terminated in edge manager" ;
    } catch( const std::exception& ex ) { 
      LOG(ERROR) << "exception nuked in edge manager - " << ex.what() ;

    }
    
    

  }

  

}
