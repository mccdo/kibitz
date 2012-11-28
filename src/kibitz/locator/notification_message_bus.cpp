#include <kibitz/locator/notification_message_bus.hpp>

namespace locator {

  const char* notification_message_bus::INPROC_BINDING = "inproc:://notification_message_bus";

  void send_notification_message( void* zmq_context, const std::string& json ) { 

  }

  notification_message_bus::notification_message_bus( void* zmq_context, const std::string& publish_binding )  {

  }

  notification_message_bus::~notification_message_bus() {
  }

  void notification_message_bus::operator()() {
  }


  
}
