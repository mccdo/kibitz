#ifndef __HEARTBEAT_SENDER_HPP__
#define __HEARTBEAT_SENDER_HPP__

#include "message_base.hpp"


namespace kibitz {

  class heartbeat_sender : public message_base {

  public:
    heartbeat_sender( context_ptr context ) ; 

    ~heartbeat_sender();  
    
  };



}



#endif
