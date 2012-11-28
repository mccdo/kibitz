#include <kibitz/locator/notification_message_bus.hpp>
#include <glog/logging.h>
#include <zmq.h>
#include <kibitz/kibitz_util.hpp>
#include <boost/thread.hpp>

namespace ku = kibitz::util;

namespace locator {

  const char* notification_message_bus::INPROC_BINDING = "inproc:://notification_message_bus";

  void send_notification_message( void* zmq_context, const std::string& json ) { 

  }

  notification_message_bus::notification_message_bus( void* zmq_context, const std::string& publish_binding, int heartbeat_frequency_ms  )
    :publish_binding_(publish_binding),
     zmq_context_( zmq_context),
     heartbeat_frequency_ms_( heartbeat_frequency_ms ) {

  }

  notification_message_bus::~notification_message_bus() {
  }

  void notification_message_bus::operator()() {
    DLOG(INFO) << "Started notification message bus" ;
    try {
      void* interthread_sock = ku::create_socket( zmq_context_, ZMQ_REP );
      ku::check_zmq( zmq_bind( interthread_sock, INPROC_BINDING ) );
      DLOG(INFO) << "Bound " << INPROC_BINDING << ".";

      boost::condition_variable condition;
      boost::mutex mutex;
      boost::unique_lock<boost::mutex> lock( mutex );
      bpt::time_duration duration = bpt::millisec( heartbeat_frequency_ms_ );

      while( true ) {
	
	while( true ) {
	  string message ;
	  if( ku::recv_async( interthread_sock, message ) ) {
	    // handle async message and reply
	  } else {
	    break;
	  } 
	}

	// heartbeat and sleep
	condition.timed_wait( lock, duration );

      }

      ku::close_socket( interthread_sock );
    } catch( const std::exception& e ) {
      LOG(ERROR) << "Notification message bus thread died. Exception => " << e.what() ;
    }
  }


  
}
