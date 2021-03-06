#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <kibitz/kibitz_util.hpp>
#include <kibitz/messages/heartbeat.hpp>
#include <kibitz/locator/heartbeat_generator.hpp>

using namespace boost::posix_time;
namespace ku = kibitz::util;

namespace kibitz
{
namespace locator
{

////////////////////////////////////////////////////////////////////////////////
heartbeat_generator::heartbeat_generator(
    const publisher& pub,
    int frequency_ms,
    int port )
    :
    publisher_( pub ),
    frequency_ms_( frequency_ms ),
    publish_port_( port ),
    m_logger( Poco::Logger::get("heartbeat_generator") )
{
    m_logStream = LogStreamPtr( new Poco::LogStream( m_logger ) );
}
////////////////////////////////////////////////////////////////////////////////
heartbeat_generator::~heartbeat_generator()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
void heartbeat_generator::operator()()
{
    KIBITZ_LOG_NOTICE( "Started heartbeat thread" );
    boost::condition_variable condition;
    boost::mutex mutex;
    boost::unique_lock< boost::mutex > lock( mutex );
    time_duration pause_duration = millisec( frequency_ms_ );
    // startup pause to give publisher time to start up
    // on other thread, avoid errno 111
    ku::wait( ku::STARTUP_PAUSE );

    util::sockman sock( publisher_.get_publish_socket() );
    heartbeat hb( publish_port_ );

    while( true )
    {
        condition.timed_wait( lock, pause_duration );
        hb.increment_tick_count() ;
        KIBITZ_LOG_DEBUG( "Sent heartbeat" );
        publisher_.send( sock, hb.to_json() );
    }
}
////////////////////////////////////////////////////////////////////////////////

} //end locator
} //end kibitz
