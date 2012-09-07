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
#ifndef kibitz_util_hpp
#define kibitz_util_hpp

#include <zmq.h>

#include <string>
#include <stdexcept>

#include <kibitz/export_config.hpp>

using std::string;

namespace kibitz {
  namespace util {

    class queue_interrupt : public std::runtime_error {
    public :
      queue_interrupt( const string& msg ) ;
      virtual ~queue_interrupt() throw(); 
    };

    KIBITZ_EXPORT void recv( void* socket, string& message ) ;
    KIBITZ_EXPORT void send( void* socket, const string& message );
    KIBITZ_EXPORT void check_zmq( int zmq_return_code ) ;
    KIBITZ_EXPORT void* create_socket( void* context, int socktype ) ;
    KIBITZ_EXPORT void close_socket( void* socket );
    KIBITZ_EXPORT void daemonize( const string& pid_file ) ;
  }
}

#endif
