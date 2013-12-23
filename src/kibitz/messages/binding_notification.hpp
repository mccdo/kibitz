#ifndef messages_binding_notification_hpp
#define messages_binding_notification_hpp

#include <kibitz/export_config.hpp>
#include <kibitz/messages/notification_message.hpp>

namespace kibitz
{
/// \brief Message that contains bind information for collaboration messages
///
///
class KIBITZ_MESSAGE_EXPORT binding_notification : public notification_message
{
    std::string target_worker_;
    std::string zmq_binding_;

public:
    /// c'tor
    ///
    /// \param target_worker name of worker type that will use this binding
    /// \param zmq_binding zmq binding where worker will get collaboration messages
    ///
    binding_notification( const std::string& target_worker, const std::string& zmq_binding );

    binding_notification( JSON::Object::Ptr json );
    virtual ~binding_notification();
    virtual std::string to_json() const;

    const std::string& target_worker() const
    {
        return target_worker_;
    }
    const std::string& binding() const
    {
        return zmq_binding_;
    }

    const static char* NOTIFICATION_TYPE;
};

typedef shared_ptr< binding_notification > binding_notification_ptr_t;
}

#endif
