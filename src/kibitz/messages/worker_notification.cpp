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
#include <kibitz/messages/worker_notification.hpp>


namespace kibitz
{

const char* worker_notification::NOTIFICATION_TYPE = "worker-notification1";

////////////////////////////////////////////////////////////////////////////////
worker_notification::worker_notification( const string& payload )
    :
    notification_message( NOTIFICATION_TYPE ),
    payload_( payload )
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
  worker_notification::worker_notification( JSON::Object::Ptr json ) 
    :
    notification_message( json )
{
  get_value( json, "payload", payload_ );
}
////////////////////////////////////////////////////////////////////////////////
string worker_notification::to_json() const
{
    stringstream stm;
    JSON::Object::Ptr json;
    read_json( "{}", json );
    notification_message::populate_header( json );
    json->set( "payload", payload_ );
    json->stringify( stm );
    return stm.str();
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz
