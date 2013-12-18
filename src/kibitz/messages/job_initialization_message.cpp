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
#include <kibitz/messages/job_initialization_message.hpp>


namespace kn = kibitz::notification ;

namespace kibitz
{

const char* job_initialization_message::NOTIFICATION_TYPE = "job_initialization";

////////////////////////////////////////////////////////////////////////////////
job_initialization_message::job_initialization_message( JSON::Object::Ptr json )
    : notification_message( json )
{
    get_value( json, kn::WORKER_TYPE, worker_type_ ) ;
    get_value( json, kn::WORKER_ID,  worker_id_ );
    get_value( json, kn::PAYLOAD,   payload_ ) ;
}
////////////////////////////////////////////////////////////////////////////////
string job_initialization_message::to_json() const
{
    stringstream stm;
    JSON::Object::Ptr json;
    read_json( "{}", json );
    notification_message::populate_header( json );
    json->set( kn::WORKER_TYPE, worker_type_ );
    json->set( kn::WORKER_ID, worker_id_ );
    json->set( kn::PAYLOAD, payload_ );
    json->stringify( stm );
    return stm.str();
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz
