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
    ptree tree;
    stringstream stm;
    collaboration_message::populate_header( tree );
    tree.put( "payload", payload_ );
    tree.put( "worker_type", worker_type_ );
    boost::property_tree::json_parser::write_json( stm, tree );
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
basic_collaboration_message::basic_collaboration_message( const ptree& json )
    :
    collaboration_message( json ),
    worker_type_( json.get< string >( "worker_type" ) ),
    payload_( json.get< string >( "payload" ) )
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
void basic_collaboration_message::to_ptree( ptree& response ) const
{
    collaboration_message::populate_header( response );
    response.put( "payload", payload_ );
    response.put( "worker_type", worker_type_ );
}
////////////////////////////////////////////////////////////////////////////////
basic_collaboration_message_ptr_t basic_collaboration_message::from_ptree(
    const ptree& tree )
{
    return basic_collaboration_message_ptr_t(
        new basic_collaboration_message( tree ) );
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz
