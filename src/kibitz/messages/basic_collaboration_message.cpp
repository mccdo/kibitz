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
#include <kibitz/messages/basic_collaboration_message.hpp>


namespace kibitz
{


////////////////////////////////////////////////////////////////////////////////
string basic_collaboration_message::to_json() const
{
    stringstream stm;
    JSON::Object::Ptr  json;
    collaboration_message::populate_header( json );
    json->set( "payload", payload_ );
    json->set( "worker_type", worker_type_ );
    json->stringify( stm );
    return stm.str();
}


  string basic_collaboration_message::to_json( JSON::Object::Ptr json ) {
    stringstream stm;
    collaboration_message::populate_header( json );
    json->set( "payload", payload_ );
    json->set( "worker_type", worker_type_ );
    json->stringify( stm );
    return stm.str();    
  }
////////////////////////////////////////////////////////////////////////////////
basic_collaboration_message::basic_collaboration_message(
    const string& worker_type,
    const string& payload )
    :
    collaboration_message( "generic" ),
    worker_type_( worker_type ),
    payload_( payload )
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
  basic_collaboration_message::basic_collaboration_message( JSON::Object::Ptr json )
    :
    collaboration_message( json )
{
  get_value( json, "worker_type", worker_type_ );
  get_value( json, "payload", payload_ );
    
}



  basic_collaboration_message_ptr_t basic_collaboration_message::create(JSON::Object::Ptr json ) {
    return basic_collaboration_message_ptr_t(
        new basic_collaboration_message( json ) );
}


////////////////////////////////////////////////////////////////////////////////

} //end kibitz
