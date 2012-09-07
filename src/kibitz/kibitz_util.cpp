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
#include <zmq.h>
#include <assert.h>
#include <fstream>
#include <sstream>

#include <kibitz/kibitz_util.hpp>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <boost/config.hpp>
#if defined(BOOST_WINDOWS)
#include <process.h>
#endif

using boost::format;

namespace fs = boost::filesystem;

using std::stringstream;
using std::runtime_error;

namespace kibitz {
  namespace util {
    queue_interrupt::queue_interrupt( const string& msg )
      :runtime_error( msg ) {
    }

    queue_interrupt::~queue_interrupt() throw() {
    }

    void daemonize( const string& pid_file ) {
      fs::path pid_path( pid_file );
      if( fs::exists( pid_path ) ) {
	throw std::runtime_error( (format("Lock file, %1% exists.") % pid_file ).str() );
      }

#ifndef BOOST_WINDOWS
      int err = daemon( 0, 0 );
      if( err ) {
#endif
	throw std::runtime_error( (format( "Could not daemonize process. errno %1%" ) % errno ).str() );
#ifndef BOOST_WINDOWS
    }
#endif
      std::fstream stm( pid_path.c_str(), std::ios::out | std::ios::trunc );
#if defined(BOOST_WINDOWS)
     stm << _getpid();
#else
     stm << getpid();
#endif
      stm.close();


    }


    void close_socket( void* socket ) {
      if( socket ) {
	zmq_close( socket );
      }
    }

    void* create_socket( void* context, int socktype ) {
      void* result = zmq_socket( context, socktype ) ;
      if( !result ) {
	stringstream stm;
	stm << "zmq_socket failed with " << zmq_errno(); 
	throw std::runtime_error( stm.str() );
      }
      return result;
    }

    void check_zmq( int return_code ) {
      if( return_code ) {
	stringstream stm;
	stm << "zmq call failed with error code " << zmq_errno() ;
	throw std::runtime_error( stm.str() );
      }
    }

    void send( void* socket, const string& message ) {
      zmq_msg_t msg;
      zmq_msg_init_size( &msg, message.length() );
      memcpy( zmq_msg_data( &msg ), message.data(), message.length() );
      zmq_sendmsg( socket, &msg, 0 );
      zmq_msg_close( &msg );
    }

    void recv( void* socket, string& message ) {
      assert( message.empty() );
      zmq_msg_t msg;
      zmq_msg_init( &msg );
      int rc = zmq_recvmsg( socket, &msg, 0 );

      if( rc < 0 ) {
	int error = zmq_errno();
	if( EINTR == error ) {
	  throw queue_interrupt( "Received interrupt" );
	} else {
	  stringstream stm;
	  stm << "Error " << error ;
	  throw runtime_error( stm.str() );
	}
      }

      int size = zmq_msg_size( &msg );
      const char* data = static_cast<const char*>( zmq_msg_data( &msg ) );
      message.append( data, size );
      zmq_msg_close( &msg );
    }
  }
}
