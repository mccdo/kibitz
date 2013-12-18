#ifndef kibitz_collaboration_handler_hpp
#define kibitz_collaboration_handler_hpp

#include <kibitz/context.hpp>
#include <kibitz/messages/collaboration_message_bundle.hpp>


namespace kibitz
{

/// Handles job collaboration message on a seperate thread.
class collaboration_handler
{
    context* context_;
    collaboration_message_bundle_ptr_t collaboration_bundle_;
public:
    /// c'tor
    ///
    /// \param ctx pointer to application context
    /// \param message bundle of collaboration messages
    ///
    collaboration_handler( context* ctx, collaboration_message_bundle_ptr_t message ) ;
    ~collaboration_handler();

    void operator()();
};
}

#endif
