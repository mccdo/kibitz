#ifndef out_edge_manager_hpp
#define out_edge_manager_hpp


#include <kibitz/zmq_socket.hpp>


namespace kibitz {
  /**
   *  
   */
  class out_edge_manager {
    context& context_;

    void handle_notification_message( void* notification_socket, socket_ptr_list_t& out_sockets ) ;
    void send_collaboration_message( void* notification_socket, const socket_ptr_list_t& out_sockets );
  public:
    out_edge_manager( context& ctx );
    virtual ~out_edge_manager();
    void operator()();
    void send( const string& message ) ;
    
  };
}





#endif
