#ifndef notification_message_bus_hpp
#define notification_message_bus_hpp

#include <string>
#include <kibitz/messages/notification_message.hpp>

namespace locator {
  /// Free function to use to send notification messages to workers  
  void send_notification_message( void* zmq_context, const std::string& json );
  
  /// Class manages thread that dispatches heartbeats and other notification messages to workers
  class notification_message_bus {
    std::string publish_binding_;
    void* zmq_context_;
    const int heartbeat_frequency_ms_;
    // binding used to communicate between
    // class worker thread and the outside
    // world
    static const char* INPROC_BINDING; 


    friend void send_notification_message( void*, const std::string& );
  public:
    notification_message_bus( void* zmq_context, const std::string& publish_binding, int heartbeat_frequency_ms = 100 ) ;
    virtual ~notification_message_bus();
    void operator()() ;
  };

}


#endif 
