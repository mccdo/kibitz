

#include <glog/logging.h>
#include <zmq.h>
#include <kibitz/kibitz_util.hpp>
#include <boost/thread.hpp>
#include <kibitz/locator/notification_message_bus.hpp>
#include <kibitz/messages/inproc_notification_message.hpp>
namespace ku = kibitz::util;
namespace k = kibitz;
using boost::dynamic_pointer_cast;

namespace locator {

  const char* notification_message_bus::INPROC_BINDING = "inproc:://notification_message_bus";

  void send_notification_message( void* zmq_context, const std::string& json ) { 

  }

  notification_message_bus::notification_message_bus( void* zmq_context, const std::string& publish_binding  )
    :publish_binding_(publish_binding),
     zmq_context_( zmq_context) { 

  }

  notification_message_bus::~notification_message_bus() {
  }



  void notification_message_bus::operator()() {
    DLOG(INFO) << "Started notification message bus" ;
    try {
      void* interthread_sock = ku::create_socket( zmq_context_, ZMQ_REP );
      ku::check_zmq( zmq_bind( interthread_sock, INPROC_BINDING ) );
      DLOG(INFO) << "Bound " << INPROC_BINDING << ".";
      
      void* publish_sock = ku::create_socket( zmq_context_, ZMQ_PUB ) ;
      ku::check_zmq( zmq_bind( publish_sock, publish_binding_.c_str() ) );

      DLOG(INFO) << "Bound notification publisher " << publish_binding_ << ".";
      bool continue_flag = true;

      while( continue_flag ) {
	string json;
	ku::recv( interthread_sock, json );
	VLOG(2) << "Received " << json ;
	k::inproc_notification_message notification_response( message::ok );
	ku::send( interthread_sock, notification_response.to_json() );

	k::notification_message_ptr_t notification_message_ptr = dynamic_pointer_cast< k::notification_message >( k::message_factory( json ) );
	
	if( notification_message_ptr->notification_type() == k::notification::INPROC_NOTIFICATION ) {
	  k::inproc_notification_ptr_t notification_ptr = dynamic_pointer_cast< inproc_notification_message >( notification_message_ptr );
	  if( notification_ptr->get_notification() == message::stop ) {
	    continue_flag = false;
	  } else {
	    continue;
	  }
	  
	}
	
	ku::send( publish_sock, notification_message_ptr->to_json() );
      }

      ku::close_socket( interthread_sock );
      ku::close_socket( publish_sock );

    } catch( const std::exception& e ) {
      LOG(ERROR) << "Notification message bus thread died. Exception => " << e.what() ;
    }
  }


  
}
