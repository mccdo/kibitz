#ifndef kibitz_collaboration_handler_hpp
#define kibitz_collaboration_handler_hpp

#include <kibitz/context.hpp>
#include <kibitz/publisher.hpp>
#include <kibitz/messages/collaboration_message_bundle.hpp>


namespace kibitz {

  /// Handles job collaboration message on a seperate thread.
  class collaboration_handler {
    context* context_;
    string binding_;
    
  public:
    /// c'tor
    /// 
    /// \param ctx pointer to application context
    /// \param message bundle of collaboration messages
    ///
    collaboration_handler( context* ctx,  const string& binding  ) ;
    void dispatch_collaboration_message_to_handler( void* inproc_publisher_socket, const string& json ) ;
    ~collaboration_handler();

    void operator() ();
  };
}

#endif
