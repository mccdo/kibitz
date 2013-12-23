
#include <kibitz/common.hpp>
#include <kibitz/publisher.hpp>
#include <kibitz/logging.hpp>

#include <kibitz/messages/message.hpp>
#include <kibitz/messages/worker_broadcast_message.hpp>
#include <kibitz/messages/inproc_notification_message.hpp>
#include <kibitz/messages/worker_notification.hpp>

namespace ku = kibitz::util;


namespace kibitz
{

//const char* publisher::INPROC_BINDING = "inproc://publisher";

////////////////////////////////////////////////////////////////////////////////
publisher::publisher(
    void* zmq_context,
    const std::string& pub_binding,
    int zmq_sock_type,
    const std::string& inproc_binding,
    publish::mode mode )
    :
    zmq_context_( zmq_context ),
    binding_( pub_binding ),
    zmq_sock_type_( zmq_sock_type ),
    inproc_binding_( inproc_binding ),
    mode_( mode ),
    m_logger( Poco::Logger::get("publisher") )
{
    m_logStream = LogStreamPtr( new Poco::LogStream( m_logger ) );
    KIBITZ_LOG_NOTICE( "Binding pubisher to " << pub_binding );
    KIBITZ_LOG_NOTICE( "Inproc publisher binding " << inproc_binding );
}
////////////////////////////////////////////////////////////////////////////////
publisher::publisher(
    void* zmq_context,
    const std::string& inproc_binding )
    :
    zmq_context_( zmq_context ),
    zmq_sock_type_( 0 ),
    inproc_binding_( inproc_binding ),
    mode_( publish::none ),
    m_logger( Poco::Logger::get("publisher") )
{
    m_logStream = LogStreamPtr( new Poco::LogStream( m_logger ) );
}
////////////////////////////////////////////////////////////////////////////////
publisher::~publisher()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
void* publisher::get_publish_socket()
{
    void* sock = util::create_socket( zmq_context_, ZMQ_REQ );
    KIBITZ_LOG_NOTICE(
            "Get publish socket, binding -> " << inproc_binding_
            << " sock address " << sock );
    util::check_zmq( zmq_connect( sock, inproc_binding_.c_str() ) );
    return sock;
}
////////////////////////////////////////////////////////////////////////////////
void publisher::send( const std::string& json )
{
    ku::sockman sock( zmq_context_, ZMQ_REQ );
    util::check_zmq( zmq_connect( sock, inproc_binding_.c_str() ) );
    util::send( sock, json );
    std::string response ;
    util::recv( sock, response );
}
////////////////////////////////////////////////////////////////////////////////
void publisher::send( void* zmq_socket, const std::string& json )
{
    util::send( zmq_socket, json );
    std::string response ;
    util::recv( zmq_socket, response );
}
////////////////////////////////////////////////////////////////////////////////
void publisher::operator()()
{
    try
    {
        KIBITZ_LOG_NOTICE( "Entered publish thread" );
        // create socket to get inproc messages
        ku::sockman inproc_ptr( zmq_context_, ZMQ_REP );
        util::check_zmq( zmq_bind( inproc_ptr, inproc_binding_.c_str() ) );
        // create socket to send messages to external subscribers
        ku::sockman publisher_ptr( zmq_context_, zmq_sock_type_ );
	const int period = 100;

        if( mode_ == publish::bind )
        {
            util::check_zmq( zmq_bind( publisher_ptr, binding_.c_str() ) );
        }
        else
        {
            util::check_zmq( zmq_connect( publisher_ptr, binding_.c_str() ) );
        }

        KIBITZ_LOG_NOTICE( "Initialized publisher thread" );

        while( true )
        {
            KIBITZ_LOG_DEBUG( "Publish loop" );
            // get message sent from other thread and respond
            std::string json;
            if( util::recv_async( inproc_ptr, json ) ) { 
	      
            inproc_notification_message response( message::ok );
            util::send( inproc_ptr, response.to_json() );
            KIBITZ_LOG_DEBUG( "Got message" );

            message_ptr_t msg = message_factory( json );

            if( msg->message_type() == NOTIFICATION_MESSAGE_TYPE )
            {
                notification_message_ptr_t notification_msg =
                    dynamic_pointer_cast< notification_message >( msg ) ;

                if( notification_msg->notification_type() ==
                        notification::INPROC_NOTIFICATION )
                {
                    inproc_notification_ptr_t inproc_msg =
                        dynamic_pointer_cast< inproc_notification_message >(
                            notification_msg );
                    if( inproc_msg->get_notification() == message::stop )
                    {
                        break;
                    }
                    // we don't pass through inproc messages to other processes
                    continue;
                }

                // if this is a worker generated notification we strip the json wrapper
                // and only send the payload since the intent of this type of message is
                // to be transmitted to agents external to kibitz
                if( notification_msg->notification_type() ==
                        worker_notification::NOTIFICATION_TYPE )
                {
                    worker_notification_ptr_t wn =
                        static_pointer_cast< worker_notification >(
                            notification_msg );
                    json = wn->get_payload() ;
                }
            }

            // publish message
            KIBITZ_LOG_DEBUG( "Published message ->" << json );
            util::send( publisher_ptr, json );
            } 
            //See if this thread has been interrupted:
            //http://www.boost.org/doc/libs/1_53_0/doc/html/thread/thread_management.html#thread.thread_management.this_thread.interruption_point
            boost::this_thread::interruption_point();

	    util::wait( period );
        }

        KIBITZ_LOG_NOTICE( "Publish thread terminated normally" );
    }
    catch( const std::exception& e )
    {
        KIBITZ_LOG_ERROR(
                "Publisher thread terminated abnormally due to error -> "
                << e.what() );
    }
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz
