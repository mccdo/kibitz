#ifndef kibitz_publisher_hpp
#define kibitz_publisher_hpp

#include <string> 
#include <kibitz/kibitz_util.hpp>



namespace kibitz {

  namespace publish {
    enum mode {
      none,
      bind,
      connect
    };
  }

  /// \brief Thread safe publisher. 
  ///
  /// Publishing takes place on a
  /// seperate thread so that other threads can send message to
  /// publisher via inproc socket which are then picked up and
  /// published.  This lets us publish from multiple threads
  /// with out dealing with synchronization issues. 
  class publisher {
    void* zmq_context_;
    const std::string binding_;
    const int zmq_sock_type_;
    const std::string inproc_binding_;
    const publish::mode mode_;
  public:
    /// c'tor 
    /// \param zmq_context zmq context obtained by zmq_init
    /// \param pub_binding binding string for publisher socket
    /// \param zmq_socket_type type of publisher ZMQ_PUB|ZMQ_PUSH|ZMQ_REQ
    /// \param inproc_binding unique binding that will allow other threads
    ///                 to communicate with the publisher in a thread safe fashion
    publisher( void* zmq_context, 
	       const std::string& pub_binding, 
	       int zmq_sock_type, 
	       const std::string& inproc_binding,
	       publish::mode mode );

    /// c'tor to use to when creating an object to send a message to be published
    /// 
    /// \param zmq_context zmq context obtained by zmq_init
    /// \param inproc_binding unique binding that will allow other threads
    ///                 to communicate with the publisher in a thread safe fashion. 
    ///                 must match inproc binding that was used to create publisher thread.
    publisher( void* zmq_context, 
	       const std::string& inproc_binding );
    ~publisher();

    /// Gets a socket that can be used to 
    /// send messages via publisher, NOTE that the
    /// returned socket is already connected.
    /// \return A connected ZMQ socket. You'll have to release the socket when you're done with it. 
    void* get_publish_socket( ) ;

    /// Publishes a message.  If you need
    /// performance you probably shouldn't use this 
    /// because it instantiates a socket for each message send 
    void send( const std::string& json ) ;
    
    void send( void* zmq_socket, const std::string& json );
    /// Thread function for publisher
    ///
    void operator()(); 
    
    void* context() { return zmq_context_; }

    /// Binding to send internal messages between threads
    //    const static char* INPROC_BINDING;
  };
}

#endif
