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
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/utility.hpp>

namespace bpt = boost::posix_time;

namespace kibitz
{
namespace util
{

static const int STARTUP_PAUSE = 1000;

class queue_interrupt : public std::runtime_error
{
public :
    queue_interrupt( const std::string& msg ) ;
    virtual ~queue_interrupt() throw();
};



/// Attempts to recieve a message and returns immediately.
///
/// \return true if a message was recieved .
///
KIBITZ_EXPORT bool recv_async( void* socket, std::string& message );

/// Indicates if a period of time has elapsed.
///
/// \param duration_millisec the time period
/// \param last_time [in,out] the last time the duration elapsed
///
/// \return true if duration has elapsed since last_time
///
KIBITZ_EXPORT bool time_elapsed( int duration_millisec, bpt::ptime& last_time ) ;
KIBITZ_EXPORT bpt::ptime get_current_local_time() ;

/// Pauses a thread for a period
/// \param duration_millisec the duration of the pause
///
KIBITZ_EXPORT void wait( int duration_millisec ) ;

KIBITZ_EXPORT void recv( void* socket, std::string& message ) ;
KIBITZ_EXPORT void send( void* socket, const std::string& message );
KIBITZ_EXPORT void check_zmq( int zmq_return_code ) ;
KIBITZ_EXPORT void* create_socket( void* context, int socktype ) ;
KIBITZ_EXPORT void close_socket( void* socket );
KIBITZ_EXPORT void daemonize( const std::string& pid_file ) ;



/// Helper class to manage and safely dispose
/// of zmq sockets using a smart pointer
class sockman : boost::noncopyable
{
    void* socket_;


public:
    sockman( void* context, int zmq_socktype )
        : socket_( create_socket( context, zmq_socktype ) )
    {
    }

    sockman( void* zmq_sock )
        : socket_( zmq_sock )
    {
    }

    ~sockman( )
    {
        close_socket( socket_ );
    }

    /// Retreives the underlying zmq socket
    /// \return The underlying ZMQ socket.
    ///
    void* get()
    {
        return socket_;
    }

    /// Conversion operator so you can pass sockman
    /// to ZMQ functions that take sockets
    operator void* ()
    {
        return socket_;
    }



};

typedef boost::shared_ptr< sockman > sockman_ptr_t;

KIBITZ_EXPORT sockman_ptr_t create_socket_ptr( void* context, int socktype ) ;

}
}

#endif
