/*************** <auto-copyright.rb BEGIN do not edit this line> **************
 *
 * Copyright 2012-2012 by Ames Laboratory
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *************** <auto-copyright.rb END do not edit this line> ***************/
#ifndef heartbeat_hpp
#define heartbeat_hpp

#include <boost/asio/ip/host_name.hpp>
#include <boost/config.hpp>
#if defined( BOOST_WINDOWS )
#include <process.h>
#endif
//#if defined( BOOST_WINDOWS )
//    return static_cast<unsigned short>(::GetCurrentProcessId());
//#else
//    return static_cast<unsigned short>(::getpid());
//#endif
#include <kibitz/messages/heartbeat.hpp>

#include <kibitz/messages/worker_notification_message.hpp>

namespace kibitz
{

////////////////////////////////////////////////////////////////////////////////
heartbeat::heartbeat( int port )
    :
    notification_message( notification::HEARTBEAT_NOTIFICATION ),
    host_name_( boost::asio::ip::host_name() ),
#if defined( BOOST_WINDOWS )
    pid_( _getpid() ),
#else
    pid_( getpid() ),
#endif
    port_( port ),
    ticks_( 0 )
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
heartbeat::heartbeat( JSON::Object::Ptr json )
    :
    notification_message( json )

{
    get_value( json, "host", host_name_ );
    get_value( json, "process_id", pid_ );
    get_value( json, "port", port_ );
    get_value( json, "ticks", ticks_ );
}
////////////////////////////////////////////////////////////////////////////////
heartbeat::~heartbeat()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
void heartbeat::increment_tick_count()
{
    static int counter = 0;
    ticks_ = ++counter;
}
////////////////////////////////////////////////////////////////////////////////
std::string heartbeat::to_json() const
{
    std::stringstream stm;
    JSON::Object::Ptr json;
    read_json( "{}", json );
    notification_message::populate_header( json );
    json->set( "host", host_name_ );
    json->set( "process_id", pid_ );
    json->set( "port" , port_ );
    json->set( "ticks", ticks_ );
    json->stringify( stm ) ;
    return stm.str();
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz

#endif
