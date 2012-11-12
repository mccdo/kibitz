#ifndef zmq_socket_hpp
#define zmq_socket_hpp

#include <kibitz/common.hpp>

namespace kibitz {


  class zmq_socket_t {
    void* socket_;
    zmq_socket_t( void* socket );

    friend shared_ptr<zmq_socket_t> create_socket_ptr( void*, int );
  public:
    virtual ~zmq_socket_t();
    operator void*() { return socket_; }
    void* get() { return socket_; }
  };
  
  typedef shared_ptr< zmq_socket_t > zmq_socket_ptr_t;
  typedef std::vector< zmq_socket_ptr_t > socket_ptr_list_t;
  zmq_socket_ptr_t create_socket_ptr( void* zmq_context, int socktype );
}


#endif
