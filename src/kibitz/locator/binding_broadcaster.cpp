
#include <kibitz/logging.hpp>

#include <kibitz/locator/binding_broadcaster.hpp>

#include <kibitz/messages/binding_notification.hpp>

#include <boost/foreach.hpp>

namespace kibitz
{
namespace locator
{

////////////////////////////////////////////////////////////////////////////////
binding_broadcaster::binding_broadcaster(
    const publisher& pub,
    binding_map_t& bindings,
    int send_frequency )
    :
    publisher_( pub ),
    bindings_( bindings ),
    send_frequency_( send_frequency ),
    m_logger( Poco::Logger::get("binding_broadcaster") )
{
    m_logStream = LogStreamPtr( new Poco::LogStream( m_logger ) );
    
    KIBITZ_LOG_NOTICE( "Instantiating binding broadcaster" );
}
////////////////////////////////////////////////////////////////////////////////
binding_broadcaster::~binding_broadcaster()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
void binding_broadcaster::operator()()
{
    try
    {
        KIBITZ_LOG_NOTICE( "Starting binding broadcast thread" );

        // wait a bit to let listener socket get started
        util::wait( util::STARTUP_PAUSE );
        util::sockman sock( publisher_.get_publish_socket() );

        KIBITZ_LOG_NOTICE( "Broadcast thread initialized, bindings will be sent every "
                << send_frequency_ << " ms." );

        while( true )
        {
            BOOST_FOREACH( const binding_pair_t binding, bindings_ )
            {
                binding_notification message( binding.first, binding.second );
                publisher_.send( sock, message.to_json() );
            }

            util::wait( send_frequency_ );
        }
    }
    catch( const std::exception& ex )
    {
        KIBITZ_LOG_ERROR( "Abnormal termination of binding broadcaster. Reason - "
                << ex.what() );
    }
}
////////////////////////////////////////////////////////////////////////////////

} //end locator
} //end kibitz
