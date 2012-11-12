#include <kibitz/zmq_socket.hpp>
#include <kibitz/kibitz_util.hpp>

namespace kibitz {
  zmq_socket_t::zmq_socket_t( void* socket ) 
    :socket_( socket ) {
  }

  zmq_socket_t::~zmq_socket_t( ) {
    util::close_socket( socket_ );
  }

  zmq_socket_ptr_t create_socket_ptr( void* zmq_context, int socktype ) {
    return zmq_socket_ptr_t( new zmq_socket_t( util::create_socket( zmq_context, socktype ) ) );
  }

}
