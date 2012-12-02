
#include <kibitz/common.hpp>
#include <kibitz/publisher.hpp>
#include <kibitz/messages/message.hpp>
#include <kibitz/messages/worker_broadcast_message.hpp>
#include <kibitz/messages/inproc_notification_message.hpp>


namespace ku = kibitz::util;


namespace kibitz {


  publisher::publisher( void* zmq_context, 
			const std::string& pub_binding, 
			int zmq_sock_type, 
			const std::string& inproc_binding )
    :zmq_context_( zmq_context ),
     binding_( pub_binding ),
     zmq_sock_type_( zmq_sock_type ),
     inproc_binding_( inproc_binding ) {
    LOG(INFO) << "Binding pubisher to " << pub_binding;
    LOG(INFO) << "Inproc publisher binding " << inproc_binding;
  }

  publisher::~publisher( ) {
  }

  void* publisher::get_publish_socket() {
    void* sock = util::create_socket( zmq_context_, ZMQ_REQ );
    DLOG(INFO) << "Get publish socket, binding -> " << inproc_binding_ << " sock address " << sock;
    util::check_zmq( zmq_connect( sock, inproc_binding_.c_str() ) );
    return sock;
  }

  void publisher::send( const std::string& json ) {
    ku::sockman sock( zmq_context_, ZMQ_REQ  );
    util::check_zmq( zmq_connect( sock, inproc_binding_.c_str() ) );
    util::send( sock, json );
    string response ;
    util::recv( sock, response );
  }

  void publisher::send( void* zmq_socket, const std::string& json ) {
    util::send( zmq_socket, json );
    string response ;
    util::recv( zmq_socket, response );

  }

  void publisher::operator()() {
    try {
      DLOG(INFO) << "Entered publish thread";
      // create socket to get inproc messages 
      ku::sockman inproc_ptr( zmq_context_, ZMQ_REP );
      util::check_zmq( zmq_bind( inproc_ptr, inproc_binding_.c_str() ));
      // create socket to send messages to external subscribers 
      ku::sockman publisher_ptr( zmq_context_, zmq_sock_type_ );
      util::check_zmq( zmq_bind( publisher_ptr, binding_.c_str() ) );
      DLOG(INFO) << "Initialized publishe thread" ;

      while( true ) {
	VLOG(1) << "Publish loop";
	// get message sent from other thread and respond
	string json;
	util::recv( inproc_ptr, json );
	inproc_notification_message response( message::ok );
	util::send( inproc_ptr, response.to_json() );
	VLOG(1) << "Got message";
	
	message_ptr_t msg = message_factory( json );

	if( msg->message_type() == NOTIFICATION_MESSAGE_TYPE ) {
	  notification_message_ptr_t notification_msg = dynamic_pointer_cast<notification_message>( msg ) ;
	  
	  if( notification_msg->notification_type() == notification::INPROC_NOTIFICATION ) {
	    inproc_notification_ptr_t inproc_msg = dynamic_pointer_cast<inproc_notification_message>(notification_msg); 
	    if( inproc_msg->get_notification() == message::stop ) {
	      break;
	    }	    
	    // we don't pass through inproc messages to other processes
	    continue;
	  }
	}
	
	// publish message
	VLOG(1) << "Published message ->" << json ;
	util::send( publisher_ptr, json );

      }

      DLOG(INFO) << "Publish thread terminated normally";

    } catch( const std::exception& e ) {
      LOG(ERROR) << "Publisher thread terminated abnormally due to error -> " << e.what() ;
    }
    
  }


}
