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
//glog must be included BEFORE zmq to build on windows
#include <glog/logging.h>

#include <zmq.h>

#include "locator.hpp"
#include <kibitz/locator/registry.hpp>
#include <kibitz/messages/heartbeat.hpp>
#include <kibitz/messages/inproc_notification_message.hpp>
#include <kibitz/kibitz_util.hpp>


#include <iostream>
#include <string>
#include <assert.h>
#include <signal.h>

#include <boost/config.hpp>
#ifdef BOOST_WINDOWS
# pragma warning(disable: 4275)
#else
#include <kibitz/GNUCompilerGuards.hpp>
GCC_DIAG_OFF( unused - parameter )
#endif

#include <boost/program_options.hpp>

#ifdef BOOST_WINDOWS
# pragma warning(default: 4275)
#else
GCC_DIAG_ON( unused - parameter )
#endif
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
using boost::format;
using namespace google;
using std::string;
namespace po = boost::program_options;

using kibitz::util::create_socket;
using kibitz::util::check_zmq;
using kibitz::util::close_socket;

string pid_file;
void signal_callback( int sig )
{
    fs::path path( pid_file );
    if( fs::exists( path ) )
    {
        fs::remove_all( path );
    }
}

int main( int argc, char* argv[] )
{

    po::options_description options( "locator" );
    options.add_options()
    ( "help,h", "Show help message" )
    ( "port,p", po::value<int>()->default_value( 5557 ), "Port used by locator to distribute notifications." )
    ( "context-threads,t", po::value<int>()->default_value( 1 ), "zmq context thread count" )
    ( "daemon,d", "Run as a daemon" )
    ( "pid-file", po::value<string>()->default_value( "/var/run/kibitz-locator.pid" ), "Location of pid file for daemon mode" )
    ;

    po::variables_map command_line;
    po::store( po::parse_command_line( argc, argv, options ), command_line );
    po::notify( command_line );

    if( command_line.count( "daemon" ) )
    {
        pid_file = command_line["pid-file"].as<string>();
        kibitz::util::daemonize( pid_file );
        signal( SIGINT, signal_callback );
    }



    InitGoogleLogging( argv[0] );
#ifndef BOOST_WINDOWS
    //This method is not implemented on windows
    InstallFailureSignalHandler();
#endif
    DLOG( INFO ) << "Start locator" ;


    const int port = command_line["port"].as<int>() ;
    int exit_code = 0;
    int rc = 0;

    if( command_line.count( "help" ) )
    {
        std::cout << options << std::endl;
        exit( 1 );
    }

    void* context = zmq_init( command_line["context-threads"].as<int>() );
    void* inproc_pub_socket = NULL;
    void* inproc_sub_socket = NULL;
    void* outsocket = NULL;
    void* insocket = NULL;

    if( !context )
    {
        LOG( ERROR ) << "Could not create zmq context " << zmq_errno();
        return 1;
    }

    try
    {
      string notification_binding = ( format( "tcp://*:%1%" ) % port ).str();

      boost::thread_group threads;
      //      locator::notification_message_bus notification_message_bus( context, notification_binding );
      // threads.create_thread( notification_message_bus );
      threads.join_all();

        insocket = create_socket( context, ZMQ_PULL );
        //check_zmq( zmq_bind( insocket, in_binding ) );
        inproc_pub_socket = create_socket( context, ZMQ_PUB );
        check_zmq( zmq_bind( inproc_pub_socket, "inproc://x" ) );
        inproc_sub_socket = create_socket( context, ZMQ_SUB );
        check_zmq( zmq_connect( inproc_sub_socket, "inproc://x" ) );
        check_zmq( zmq_setsockopt( inproc_sub_socket, ZMQ_SUBSCRIBE, "", 0 ) );
        outsocket = create_socket( context, ZMQ_PUB );

        //LOG( INFO ) << "Locator will publish on " << pub_binding;
        //check_zmq( zmq_bind( outsocket, pub_binding.c_str() ) );

        registry reg( inproc_pub_socket, inproc_sub_socket, outsocket );
        boost::thread sender_thread( reg );

        try
        {
            while( true )
            {
                string message;
                kibitz::util::recv( insocket, message );
                if( !message.empty() )
                {
                    DLOG( INFO ) << "got message -> " << message ;
                    reg.push_message( message );
                }
            }
        }
        catch( const kibitz::util::queue_interrupt& )
        {
            LOG( INFO ) << "Caught signal shutting down" ;
        }
        catch( const std::exception& ex )
        {
            LOG( ERROR ) << "Something bad killed us. What => " << ex.what() ;
            exit_code = 1;
        }

        kibitz::inproc_notification_message inproc_notification_message( kibitz::message::stop );
        reg.push_message( inproc_notification_message );
        sender_thread.join();


    }
    catch( const std::exception& ex )
    {
        exit_code = 1;
        LOG( ERROR ) << "An exception killed worker locator " << ex.what() ;
    }

    close_socket( inproc_pub_socket );
    close_socket( inproc_sub_socket );
    close_socket( outsocket );
    close_socket( insocket ) ;
    zmq_term( context );



    return exit_code;

}


