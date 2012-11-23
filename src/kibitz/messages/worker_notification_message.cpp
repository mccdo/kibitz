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

worker_notification_message::worker_notification_message( const ptree& json )
    : notification_message( "worker_notification" ),
      worker_type_( json.get<string>( "worker_type" ) ),
      worker_id_( json.get<int32_t>( "worker_id" ) ),
      host_name_( json.get<string>( "host_name" ) ),
      port_( json.get<int16_t>( "port" ) )
{
}

worker_notification_message::worker_notification_message( heartbeat_ptr_t hb )
    : notification_message( "worker_notification" ),
      worker_type_( hb->worker_type_ ),
      worker_id_( hb->worker_id_ ),
      host_name_( hb->host_name_ ),
      port_( PORT_UNASSIGNED )
{
}

worker_notification_message::worker_notification_message( const string& worker_type )
    : notification_message( "worker_notification" ),
      worker_type_( worker_type ),
      worker_id_( WORKER_ID_UNASSIGNED ),
      port_( PORT_UNASSIGNED )
{
}

const string& worker_notification_message::worker_type() const
{
    return worker_type_;
}

const string& worker_notification_message::host_name() const
{
    return host_name_;
}

int32_t worker_notification_message::worker_id() const
{
    return worker_id_;
}

int16_t worker_notification_message::port() const
{
    return port_;
}

string worker_notification_message::to_json() const
{
    stringstream stm;
    ptree tree;
    notification_message::populate_header( tree );
    tree.put( "worker_type", worker_type_ );
    tree.put( "worker_id", worker_id_ );
    tree.put( "host_name", host_name_ );
    tree.put( "port", port_ );
    boost::property_tree::json_parser::write_json( stm, tree );
    return stm.str();
}



}
