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
#include <kibitz/messages/inproc_notification_message.hpp>


namespace kibitz
{

////////////////////////////////////////////////////////////////////////////////
inproc_notification_message::inproc_notification_message( int notification )
    :
    notification_message( "inproc" ),
    notification_( notification )
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
inproc_notification_message::inproc_notification_message( JSON::Object::Ptr json )
    :
    notification_message( json )
{
    get_value( json, "notification", notification_ );
}
////////////////////////////////////////////////////////////////////////////////
std::string inproc_notification_message::to_json() const
{
    std::stringstream stm;
    JSON::Object::Ptr json;
    read_json( "{}", json );
    notification_message::populate_header( json );
    json->set( "notification", notification_ );
    json->stringify( stm );
    return stm.str();
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz
