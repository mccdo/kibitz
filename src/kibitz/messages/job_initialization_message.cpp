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

job_initialization_message::job_initialization_message( const ptree& json )
    : notification_message( NOTIFICATION_TYPE ),
      worker_type_( json.get<string>( kn::WORKER_TYPE ) ),
      worker_id_( json.get<int>( kn::WORKER_ID ) ),
      payload_( json.get<string>( kn::PAYLOAD ) )
{
}



string job_initialization_message::to_json() const
{
    stringstream stm;
    ptree tree;
    notification_message::populate_header( tree );
    tree.put( kn::WORKER_TYPE, worker_type_ );
    tree.put( kn::WORKER_ID, worker_id_ );
    tree.put( kn::PAYLOAD, payload_ );
    boost::property_tree::json_parser::write_json( stm, tree );
    return stm.str();
}

}
