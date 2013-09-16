#include <kibitz/messages/binding_notification.hpp>

namespace kibitz
{

const char* binding_notification::NOTIFICATION_TYPE = "binding";

////////////////////////////////////////////////////////////////////////////////
binding_notification::binding_notification(
    const string& target_worker,
    const string& zmq_binding )
    :
    notification_message( binding_notification::NOTIFICATION_TYPE ),
    target_worker_( target_worker ),
    zmq_binding_( zmq_binding )
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
  binding_notification::binding_notification( JSON::Object::Ptr json )
    :
    notification_message( binding_notification::NOTIFICATION_TYPE )
{
  get_value( json, "target_worker", target_worker_ );
  get_value( json, "zmq_binding", zmq_binding_ );

}
////////////////////////////////////////////////////////////////////////////////
binding_notification::~binding_notification()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
string binding_notification::to_json() const
{
    stringstream stm;
    JSON::Object::Ptr json;
    notification_message::populate_header( json );
    json->set( "target_worker", target_worker_ );
    json->set( "zmq_binding", zmq_binding_ );
    json->stringify( stm );
    return stm.str();
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz
