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
#include <kibitz/messages/worker_broadcast_message.hpp>

namespace kibitz
{

////////////////////////////////////////////////////////////////////////////////
  worker_broadcast_message::worker_broadcast_message( JSON::Object::Ptr json )
    :
    notification_message( "worker_broadcast" )
{
  get_value( json, "notification", notification_ );
}
////////////////////////////////////////////////////////////////////////////////
worker_broadcast_message::worker_broadcast_message( const string& notification )
    :
    notification_message( "worker_broadcast" ),
    notification_( notification )
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
string worker_broadcast_message::to_json() const
{
    stringstream stm;
    JSON::Object::Ptr json; 
    notification_message::populate_header( json ); 
    json->set( "notification", notification_ );
    json->stringify( stm ) ; 
    return stm.str();
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz
