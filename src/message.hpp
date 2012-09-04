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

#include <sstream>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <string>

#include <stdint.h>

using boost::property_tree::ptree;
using boost::shared_ptr;
using std::string;
using std::stringstream;

namespace kibitz {

  namespace notification {
    static const char* WORKER_TYPE = "worker_type";
    static const char* WORKER_ID = "worker_id";
  } 
  

  class message {
    const string message_type_;
    const string version_;
  protected :
    message( const string& message_type, const string& version  )
      :message_type_(message_type),
       version_( version )  {}
    message( const ptree& json ) 
      :message_type_( json.get<string>("message_type") ),
       version_( json.get<string>("version") ) {}
    virtual void populate_header( ptree& tree ) const {
      tree.put( "message_type", message_type_ );
      tree.put( "version", version_ );
    }
  public:
    static const int16_t PORT_UNASSIGNED = 0x7FFF;
    static const int32_t WORKER_ID_UNASSIGNED = 0x7FFFFFFFL;

    virtual ~message() {}
    static const int stop = 0;
    static const int ok = 0;
    virtual string to_json() const = 0;
    const string& message_type() const { return message_type_; }
    const string& version() const { return version_ ; }
  };


  typedef shared_ptr<message> message_ptr_t;

  message_ptr_t message_factory( const string& json ) ;
}


#endif
