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
#include <kibitz/logging.hpp>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/config.hpp>
#if defined( BOOST_WINDOWS )
#include <process.h>
#endif

using boost::format;

namespace fs = boost::filesystem;
namespace bpt = boost::posix_time;

using std::runtime_error;

namespace kibitz
{
namespace util
{

////////////////////////////////////////////////////////////////////////////////
queue_interrupt::queue_interrupt( const std::string& msg )
    :
    runtime_error( msg )
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
queue_interrupt::~queue_interrupt() throw()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
bpt::ptime get_current_local_time()
{
    return bpt::microsec_clock::local_time();
}
////////////////////////////////////////////////////////////////////////////////
bool time_elapsed( int duration_millisec, bpt::ptime& last_time )
{
    bool elapsed = false;
    const int microsec_in_millisec = 10000;
    bpt::ptime current_time( get_current_local_time() );
    bpt::time_duration duration = current_time - last_time;
    if( duration.total_microseconds() >
            ( duration_millisec * microsec_in_millisec ) )
    {
        last_time = current_time;
        elapsed = true;
    }
    return elapsed;
}
////////////////////////////////////////////////////////////////////////////////
void daemonize( const std::string& pid_file )
{
    fs::path pid_path( pid_file );
    if( fs::exists( pid_path ) )
    {
        throw std::runtime_error(
            ( boost::format( "Lock file, %1% exists." ) % pid_file ).str() );
    }

#ifndef BOOST_WINDOWS
    int err = daemon( 0, 0 );
    if( err )
    {
#endif
        throw std::runtime_error(
            ( boost::format( "Could not daemonize process. errno %1%" ) % errno ).str() );
#ifndef BOOST_WINDOWS
    }
#endif
    std::fstream stm( pid_path.c_str(), std::ios::out | std::ios::trunc );
#if defined( BOOST_WINDOWS )
    stm << _getpid();
#else
    stm << getpid();
#endif
    stm.close();
}
////////////////////////////////////////////////////////////////////////////////
void wait( int duration_millisec )
{
    boost::condition_variable condition;
    boost::mutex mutex;
    boost::unique_lock< boost::mutex > lock( mutex );
    boost::posix_time::time_duration pause_duration =
        boost::posix_time::millisec( duration_millisec );
    condition.timed_wait( lock, pause_duration );
}
////////////////////////////////////////////////////////////////////////////////
void close_socket( void* socket )
{
    if( socket )
    {
        zmq_close( socket );
    }
}
////////////////////////////////////////////////////////////////////////////////
void* create_socket( void* context, int socktype )
{
    void* result = zmq_socket( context, socktype ) ;
    if( !result )
    {
        std::stringstream stm;
        stm << "zmq_socket failed with " << zmq_errno();
        throw std::runtime_error( stm.str() );
    }
    return result;
}
////////////////////////////////////////////////////////////////////////////////
void check_zmq( int return_code )
{
    if( return_code )
    {
        int error = zmq_errno();
        std::stringstream stm;
        stm << "ZMQ call failed with error code " << error;
        stm << ". Description -> " << zmq_strerror( error );
        throw std::runtime_error( stm.str() );
    }
}
////////////////////////////////////////////////////////////////////////////////
void send( void* socket, const std::string& message )
{
    zmq_msg_t msg;
    zmq_msg_init_size( &msg, message.length() );
    memcpy( zmq_msg_data( &msg ), message.data(), message.length() );
    int rc = zmq_sendmsg( socket, &msg, 0 );
    if( rc == -1 )
    {
        KIBITZ_STATIC_LOG_ERROR( "kibits_util::send", "SEND FAILED: " << zmq_strerror( zmq_errno() ) );
    }
    zmq_msg_close( &msg );
}
////////////////////////////////////////////////////////////////////////////////
bool recv_async( void* socket, std::string& message )
{
    assert( message.empty() );
    bool message_arrived = false;
    zmq_msg_t msg;
    zmq_msg_init( &msg );
    int rc = zmq_recvmsg( socket, &msg, ZMQ_DONTWAIT );

    if( rc < 0 )
    {
        int err = zmq_errno();
        if( EINTR == err )
        {
            throw queue_interrupt( "Received interrupt" );
        }
        // EAGAIN indicates no message available to be read, but
        // no error exists either, everything else throw
        if( EAGAIN != err )
        {
            throw std::runtime_error(
                ( boost::format( "Error reading queue. Err = %1%" ) % err ).str() );
        }
    }
    else
    {
        message_arrived = true;
    }

    return message_arrived;
}
////////////////////////////////////////////////////////////////////////////////
void recv( void* socket, std::string& message )
{
    assert( message.empty() );
    zmq_msg_t msg;
    zmq_msg_init( &msg );
    int rc = zmq_recvmsg( socket, &msg, 0 );

    if( rc < 0 )
    {
        int error = zmq_errno();
        if( EINTR == error )
        {
            throw queue_interrupt( "Received interrupt" );
        }
        else
        {
            std::stringstream stm;
            stm << "Error " << error ;
            throw runtime_error( stm.str() );
        }
    }

    int size = zmq_msg_size( &msg );
    const char* data = static_cast< const char* >( zmq_msg_data( &msg ) );
    message.append( data, size );
    zmq_msg_close( &msg );
}
////////////////////////////////////////////////////////////////////////////////
sockman_ptr_t create_socket_ptr( void* context, int socktype )
{
    sockman_ptr_t ptr = sockman_ptr_t( new sockman( context, socktype ) );
    return ptr;
}
////////////////////////////////////////////////////////////////////////////////

} //end util
} //end kibitz
