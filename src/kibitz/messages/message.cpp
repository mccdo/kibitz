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

#include <kibitz/messages/message.hpp>

#include <kibitz/messages/heartbeat.hpp>
#include <kibitz/messages/inproc_notification_message.hpp>
#include <kibitz/messages/worker_notification_message.hpp>
#include <kibitz/messages/worker_query_response.hpp>
#include <kibitz/messages/worker_query.hpp>
#include <kibitz/messages/worker_broadcast_message.hpp>
#include <kibitz/messages/job_initialization_message.hpp>

#include <kibitz/messages/basic_collaboration_message.hpp>

namespace kibitz
{

message_ptr_t notification_message_factory( const ptree& tree )
{
    message_ptr_t result;

    const string notification_type = tree.get<string>( "notification_type" );
    if( notification_type == "heartbeat" )
    {
        result = message_ptr_t( new heartbeat( tree ) );
    }

    if( notification_type == "inproc" )
    {
        result = message_ptr_t( new inproc_notification_message( tree ) );
    }

    if( notification_type == "worker_broadcast" )
    {
        result = message_ptr_t( new worker_broadcast_message( tree ) );
    }

    if( notification_type == "worker_notification" )
    {
        result = message_ptr_t( new worker_notification_message( tree ) );
    }

    if( notification_type == "worker_query_response" )
    {
        result = message_ptr_t( new worker_query_response( tree ) );
    }

    if( notification_type == "worker_query" )
    {
        result = message_ptr_t( new worker_query( tree ) );
    }

    if( notification_type == notification::JOB_INITIALIZATION )
    {
        result = message_ptr_t( new job_initialization_message( tree ) );
    }

    return result;
}

message_ptr_t collaboration_message_factory( const ptree& tree )
{
    message_ptr_t result;

    const string collaboration_type = tree.get<string>( "collaboration_type" );
    if( collaboration_type == "generic" )
    {
        result =  message_ptr_t( new basic_collaboration_message( tree ) );
    }

    return result;
}

message_ptr_t message_factory( const string& json )
{
    message_ptr_t result;
    stringstream sstm;
    sstm << json;
    ptree tree;
    boost::property_tree::json_parser::read_json( sstm, tree );
    const string message_type = tree.get<string>( "message_type" );

    if( message_type == "notification" )
    {
        result = notification_message_factory( tree );
    }

    if( message_type == "collaboration" )
    {
        result = collaboration_message_factory( tree );
    }

    return result;
}

}
