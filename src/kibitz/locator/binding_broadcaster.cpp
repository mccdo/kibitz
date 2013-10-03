
#include <glog/logging.h>

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
    send_frequency_( send_frequency )
{
    DLOG( INFO ) << "Instantiating binding broadcaster";
}
////////////////////////////////////////////////////////////////////////////////
binding_broadcaster::~binding_broadcaster()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
void binding_broadcaster::operator ()()
{
    try
    {
        LOG( INFO ) << "Starting binding broadcast thread";
        // wait a bit to let listener socket get started
        util::wait( util::STARTUP_PAUSE );
        util::sockman sock( publisher_.get_publish_socket() );

        LOG( INFO )
            << "Broadcast thread initialized, bindings will be sent every "
            << send_frequency_ << " ms.";

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
        LOG( ERROR )
            << "Abnormal termination of binding broadcaster. Reason - "
            << ex.what();
    }
}
////////////////////////////////////////////////////////////////////////////////

} //end locator
} //end kibitz
