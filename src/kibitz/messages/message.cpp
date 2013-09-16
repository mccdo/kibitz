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
#include <kibitz/messages/worker_broadcast_message.hpp>
#include <kibitz/messages/job_initialization_message.hpp>
#include <kibitz/messages/binding_notification.hpp>
#include <kibitz/messages/basic_collaboration_message.hpp>
#include <kibitz/messages/collaboration_message_bundle.hpp>
#include <kibitz/messages/worker_notification.hpp>

#include <Poco/JSON/ParseHandler.h>

#include <boost/config.hpp>
#ifdef BOOST_WINDOWS
#define GLOG_NO_ABBREVIATED_SEVERITIES 1
#endif
#include <glog/logging.h>

namespace kibitz
{

////////////////////////////////////////////////////////////////////////////////
  message_ptr_t notification_message_factory( JSON::Object::Ptr json  )
{
    message_ptr_t result;

    string notification_type;
    get_value( json,  "notification_type", notification_type  );
    
    if( notification_type == "heartbeat" )
    {
        result = message_ptr_t( new heartbeat( json ) );
    }

    if( notification_type == "inproc" )
    {
        result = message_ptr_t( new inproc_notification_message( json ) );
    }

    if( notification_type == "worker_broadcast" )
    {
        result = message_ptr_t( new worker_broadcast_message( json ) );
    }

    if( notification_type == "worker_notification" )
    {
        result = message_ptr_t( new worker_notification_message( json ) );
    }

    if( notification_type == job_initialization_message::NOTIFICATION_TYPE )
    {
        result = message_ptr_t( new job_initialization_message( json ) );
    }

    if( notification_type == binding_notification::NOTIFICATION_TYPE )
    {
        result = message_ptr_t( new binding_notification( json ) );
    }

    if( notification_type == worker_notification::NOTIFICATION_TYPE )
    {
        result = message_ptr_t( new worker_notification( json ) );
    }

    return result;
}
////////////////////////////////////////////////////////////////////////////////
  message_ptr_t collaboration_message_factory( JSON::Object::Ptr json  )
{
    message_ptr_t result;

    string collaboration_type ;
    get_value( json, "collaboration_type", collaboration_type ); 
    
    if( collaboration_type == "generic" )
    {
        result = message_ptr_t( new basic_collaboration_message( json ) );
    }

    if( collaboration_type == collaboration_message_bundle::MESSAGE_TYPE )
    {
      result = message_ptr_t( new collaboration_message_bundle( json ) );
    }

    return result;
}
////////////////////////////////////////////////////////////////////////////////
message_ptr_t message_factory( const string& json ) {
  message_ptr_t result;
  stringstream sstm;
  sstm << json;
  ptree tree;
  VLOG( 1 ) << "RAW MESSAGE [" << json << "]";

  JSON::Object::Ptr parsed;
  read_json( json, parsed );

  string message_type; 
  get_value( parsed, "message_type", message_type );

  if( message_type == "notification" )
    {
      result = notification_message_factory( parsed );
    }

  if( message_type == "collaboration" )
    {
      result = collaboration_message_factory( parsed );
    }


  return result;
}
////////////////////////////////////////////////////////////////////////////////


  void read_json( const string& json, JSON::Object::Ptr& ptr ) {
    JSON::Parser parser;
    Dynamic::Var result = parser.parse( json ) ;
    ptr = result.extract<JSON::Object::Ptr>();
  }

} //end kibitz
