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
heartbeat::heartbeat( const ptree& json )
    :
    notification_message( notification::HEARTBEAT_NOTIFICATION ),
    host_name_( json.get< string >( "host" ) ),
    pid_( json.get< int >( "process_id" ) ),
    port_( json.get< int >( "port" ) ),
    ticks_( json.get< int >( "ticks" ) )
{
    ;
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
string heartbeat::to_json() const
{
    stringstream stm;
    ptree tree;
    notification_message::populate_header( tree );
    tree.put( "host", host_name_ );
    tree.put( "process_id", pid_ );
    tree.put( "port" , port_ );
    tree.put( "ticks", ticks_ );
    boost::property_tree::json_parser::write_json( stm, tree );
    return stm.str();
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz

#endif
