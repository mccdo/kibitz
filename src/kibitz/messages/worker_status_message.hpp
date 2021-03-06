#ifndef worker_status_message_hpp
#define worker_status_message_hpp

#include <kibitz/messages/notification_message.hpp>

namespace kibitz
{

enum worker_status_t
{
    START = 0,
    STOP = 1,
    WORK_RECIEVED = 2,
    WORK_SENT = 3
} ;
/// \brief Workers may optionally broadcast this message to indicate occurrance of certain events
///
class KIBITZ_MESSAGE_EXPORT worker_status_message : public notification_message
{
    std::string worker_type_;
    std::string worker_id_;
    worker_status_t status_;
    std::string timestamp_;
public:
    worker_status_message( const std::string& worker_type, const std::string& worker_id, worker_status_t status );
    worker_status_message( JSON::Object::Ptr json );
    virtual ~worker_status_message() {}
    virtual std::string to_json() const;

    const std::string& worker_type() const
    {
        return worker_type_;
    }
    const std::string& worker_id() const
    {
        return worker_id_;
    }
    const worker_status_t status() const
    {
        return status_;
    }
    const std::string& timestamp() const
    {
        return timestamp_;
    }

    static const char* NOTIFICATION_TYPE;
};

typedef shared_ptr<worker_status_message> worker_status_message_ptr_t;

}

#endif // worker_status_message_hpp
