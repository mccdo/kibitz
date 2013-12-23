#ifndef kibitz_collaboration_handler_hpp
#define kibitz_collaboration_handler_hpp

#include <kibitz/context.hpp>
#include <kibitz/publisher.hpp>
#include <kibitz/messages/collaboration_message_bundle.hpp>
#include <kibitz/logging.hpp>


namespace kibitz
{

/// Handles job collaboration message on a seperate thread.
class collaboration_handler
{
    context* context_;
  std::string binding_;
    
  public:
    /// c'tor
    ///
    /// \param ctx pointer to application context
    /// \param binding inproc zmq binding that will be used to recieve messages from in_edge_manager
    ///
  collaboration_handler( context* ctx,  const std::string& binding  ) ;
    ~collaboration_handler();

    void operator()();
private:
    ///Logger utilities for debugging
    Poco::Logger& m_logger;
    ///Logger utilities for debugging
    LogStreamPtr m_logStream;
};
}

#endif
