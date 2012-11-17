#include <kibitz/out_edge_manager.hpp>

#include <kibitz/bus.hpp>
#include <kibitz/kibitz_util.hpp>
#include <kibitz/context.hpp>
#include <kibitz/worker_map.hpp>

#include <kibitz/messages/worker_broadcast_message.hpp>
#include <kibitz/messages/basic_collaboration_message.hpp>
#include <kibitz/messages/job_initialization_message.hpp>

#include <kibitz/validator/worker_graph.hpp>
#include <kibitz/validator/worker_graph.hpp>

namespace wg = kibitz::graph;

namespace kibitz {

  out_edge_manager::out_edge_manager( context& ctx ) 
    :context_( ctx ) {
    
  }

  out_edge_manager::~out_edge_manager() { 
  }


  void out_edge_manager::handle_notification_message( void* notification_socket, socket_ptr_list_t& out_sockets ) {
    DLOG( INFO ) << "Out edge manager handling notification" ;
    string json;
    util::recv( notification_socket, json );
    DLOG( INFO ) << "JSON " << json ;
    notification_message_ptr_t notification_message_ptr = dynamic_pointer_cast<notification_message>( message_factory( json ) );
    string notification_type = notification_message_ptr->notification_type();
 
    if( notification_type == notification::WORKER_BROADCAST ) {
      worker_broadcast_message_ptr_t broadcast_ptr = dynamic_pointer_cast<worker_broadcast_message>( notification_message_ptr );
      string notification = broadcast_ptr->notification();
 
     // create publications for out edges, we have a publication for each worker type, since there can 
      // be multiple instances of each worker type, this will set up fair queueing for instances of a type
      if( notification == notification::CREATE_BINDINGS ) {
	DLOG( INFO ) <<  "Creating bindings for out edges";
	create_bindings( broadcast_ptr->payload(), out_sockets );
      }
      
    }    
  }
  
  void out_edge_manager::create_bindings( const string& binding_info, socket_ptr_list_t& out_sockets ) {
    wg::worker_graph_ptr worker_graph_ptr = wg::create_worker_graph_from_string( binding_info );
    wg::node_ptr_t worker_ptr = worker_graph_ptr->get_worker( context_.worker_type() );
    
    if( worker_ptr == NULL ) {
      LOG( ERROR ) << "Attempt to create out edge bindings failed because \
collaboration graph does not contain a worker named " << context_.worker_type() << ".";
      return;
    }

    // free up sockets 
    out_sockets.clear();
    
    
    

  }

  void out_edge_manager::send_collaboration_message( void* notification_socket, const socket_ptr_list_t& out_sockets ) {

  }

  void out_edge_manager::operator()() {
    LOG( INFO ) << "out edge manager thread started";
    const int count_items = 2;
    zmq_pollitem_t pollitems[ count_items ];
    const int HEARTBEAT_SOCKET = 0;
    const int COLLABORATION_SOCKET = 1;

    try {
      // TODO: fix this if broadcast subscriber is created before broadcast publisher
      //       an exception errno 111 is raised, thus this sleep hack
      //       we want to give the publisher time to be instantiated
#ifdef BOOST_WINDOWS
      Sleep( 5000 );
#else
      sleep( 5 );
#endif
      sub broadcast_subscriber( context_.zmq_context(), HEARTBEAT_RECEIVER_BROADCASTS ) ; 
      sub collaboration_subscriber( context_.zmq_context(), INPROC_COLLABORATION_MESSAGE );
      pollitems[HEARTBEAT_SOCKET].socket = broadcast_subscriber.socket() ;
      pollitems[HEARTBEAT_SOCKET].fd = 0;
      pollitems[HEARTBEAT_SOCKET].events = ZMQ_POLLIN;
      pollitems[HEARTBEAT_SOCKET].revents = 0;
      pollitems[COLLABORATION_SOCKET].socket = collaboration_subscriber.socket() ;
      pollitems[COLLABORATION_SOCKET].fd = 0;
      pollitems[COLLABORATION_SOCKET].events = ZMQ_POLLIN;
      pollitems[COLLABORATION_SOCKET].revents = 0;
      DLOG( INFO ) << "out edge thread initialized";

      socket_ptr_list_t out_sockets;

      while( true ) {
	int rc = zmq_poll( pollitems, count_items, -1 );
	if( rc > 0 ) {
	  if( pollitems[HEARTBEAT_SOCKET].revents & ZMQ_POLLIN ) {
	    handle_notification_message( pollitems[HEARTBEAT_SOCKET].socket, out_sockets );
	  }

	  if( pollitems[COLLABORATION_SOCKET].revents & ZMQ_POLLIN ) {
	    send_collaboration_message( pollitems[COLLABORATION_SOCKET].socket, out_sockets );
	  } 
	}
      }
      
    } catch( const util::queue_interrupt& ) {
      LOG( INFO ) << "Interrupt handled out edge manager shutting down";
    } catch( const std::exception& ex ) {
      LOG( ERROR ) << "Exception in out edge manager - " << ex.what() ;
    }
  }

  void out_edge_manager::send( const string& message ) {

  }


}
