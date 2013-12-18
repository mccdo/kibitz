#include <kibitz/messages/worker_status_message.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;
namespace kibitz
{

const char* worker_status_message::NOTIFICATION_TYPE = "worker_status_message";

worker_status_message::worker_status_message( const string& worker_type, const string& worker_id, worker_status_t status )
    : notification_message( worker_status_message::NOTIFICATION_TYPE ),
      worker_type_( worker_type ),
      worker_id_( worker_id ),
      status_( status ),
      timestamp_( to_iso_extended_string( microsec_clock::universal_time() ) )
{
}

worker_status_message::worker_status_message( JSON::Object::Ptr json )
    : notification_message( json )
{
    get_value( json, "worker_type", worker_type_ );
    get_value( json, "worker_id", worker_id_ );
    int status;
    get_value( json, "status", status );
    status_ = static_cast<worker_status_t>( status );
    get_value( json, "timestamp", timestamp_ );
}

string worker_status_message::to_json() const
{
    stringstream stm;
    JSON::Object::Ptr json;
    read_json( "{}", json );
    notification_message::populate_header( json ) ;
    json->set( "worker_type", worker_type_ );
    json->set( "worker_id", worker_id_ );
    json->set( "status", static_cast<int>( status_ ) );
    json->set( "timestamp", timestamp_ ) ;
    json->stringify( stm );
    return stm.str() ;
}


}
