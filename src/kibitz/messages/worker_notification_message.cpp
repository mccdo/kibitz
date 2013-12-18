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
#include <kibitz/messages/worker_notification_message.hpp>


namespace kibitz
{

////////////////////////////////////////////////////////////////////////////////
worker_notification_message::worker_notification_message( JSON::Object::Ptr json )
    :
    notification_message( json )

{
    get_value( json, "worker_type", worker_type_ );
    get_value( json, "worker_id",   worker_id_ );
    get_value( json, "host_name",   host_name_ ) ;
    get_value( json, "port",   port_ );

}
////////////////////////////////////////////////////////////////////////////////
worker_notification_message::worker_notification_message( heartbeat_ptr_t hb )
    :
    notification_message( "worker_notification" ),
    worker_type_( "unassigned" ),
    worker_id_( WORKER_ID_UNASSIGNED ),
    host_name_( hb->host_name_ ),
    port_( hb->port_ )
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
worker_notification_message::worker_notification_message(
    const string& worker_type )
    :
    notification_message( "worker_notification" ),
    worker_type_( worker_type ),
    worker_id_( WORKER_ID_UNASSIGNED ),
    port_( PORT_UNASSIGNED )
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
const string& worker_notification_message::worker_type() const
{
    return worker_type_;
}
////////////////////////////////////////////////////////////////////////////////
const string& worker_notification_message::host_name() const
{
    return host_name_;
}
////////////////////////////////////////////////////////////////////////////////
int32_t worker_notification_message::worker_id() const
{
    return worker_id_;
}
////////////////////////////////////////////////////////////////////////////////
int16_t worker_notification_message::port() const
{
    return port_;
}
////////////////////////////////////////////////////////////////////////////////
string worker_notification_message::to_json() const
{
    stringstream stm;
    JSON::Object::Ptr json;
    read_json( "{}", json );
    notification_message::populate_header( json );
    json->set( "worker_type", worker_type_ );
    json->set( "worker_id", worker_id_ );
    json->set( "host_name", host_name_ );
    json->set( "port", port_ );
    json->stringify( stm );
    return stm.str();
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz
