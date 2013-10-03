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
#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>

#include <kibitz/export_config.hpp>

#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Handler.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Array.h>

#include <sstream>
#include <string>

#ifdef BOOST_WINDOWS

typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

#else
#include <stdint.h>
#endif

using namespace Poco;
using boost::shared_ptr;
using std::string;
using std::stringstream;

namespace kibitz
{

namespace notification
{
static const char* WORKER_TYPE = "worker_type";
static const char* WORKER_ID = "worker_id";
static const char* PAYLOAD = "payload";
  static const char* INPROC_NOTIFICATION = "inproc";
  static const char* HEARTBEAT_NOTIFICATION = "heartbeat";
  static const char* WORKER_BROADCAST = "worker_broadcast";
  static const char* SHUTDOWN_NOTIFICATION = "shutdown";
  
}


  static const char* NOTIFICATION_MESSAGE_TYPE = "notification";
  static const char* COLLABORATION_MESSAGE_TYPE = "collaboration";


template< typename T > void get_value( JSON::Object::Ptr p, const string& key, T& value ) {
  Dynamic::Var field = p->get( key ) ;
  if( field.isEmpty() ) {
    throw std::runtime_error( (boost::format("Key %1% was not found. %2% %3%") % key % __FILE__ % __LINE__ ).str() );
  }
  value = field.extract<T>();
}


class KIBITZ_MESSAGE_EXPORT message
{
    string message_type_;
    string version_;
protected :
    message( const string& message_type, const string& version )
        : message_type_( message_type ),
          version_( version )  {}
    message( JSON::Object::Ptr json ) {
      get_value( json, "message_type", message_type_ );
      get_value( json, "version" , version_ );
    }

    virtual void populate_header( JSON::Object::Ptr json ) const
    {
      json->set( "message_type", message_type_ );
      json->set( "version", version_ ) ;
    }
public:
    static const int16_t PORT_UNASSIGNED = 0x7FFF;
    static const int32_t WORKER_ID_UNASSIGNED = 0x7FFFFFFFL;

    virtual ~message() {}
    static const int stop = 0;
    static const int ok = 0;
    virtual string to_json() const = 0;
    const string& message_type() const
    {
        return message_type_;
    }
    const string& version() const
    {
        return version_ ;
    }
};


typedef shared_ptr<message> message_ptr_t;

KIBITZ_MESSAGE_EXPORT message_ptr_t message_factory( const string& json ) ;

KIBITZ_MESSAGE_EXPORT void read_json( const string& json, JSON::Object::Ptr& ptr );


  
}


#endif
