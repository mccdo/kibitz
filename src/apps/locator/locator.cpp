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
#include <kibitz/locator/heartbeat_generator.hpp>
#include <kibitz/validator/validator.hpp>

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

using kibitz::locator::heartbeat_generator;
namespace kg = kibitz::graph;

string pid_file;
void signal_handler( int, siginfo_t*, void* ) {
  fs::path path( pid_file );
  if( fs::exists(  path ) ) {
    fs::remove_all( path );
  }
  raise( SIGINT );
}

int main( int argc, char* argv[] )
{

    po::options_description options( "locator" );
    options.add_options()
    ( "help,h", "Show help message" )
    ( "port,p", po::value<int>()->default_value( 5557 ), "Port used by locator to distribute heartbeats." )
    ( "context-threads,t", po::value<int>()->default_value( 1 ), "zmq context thread count" )
    ( "daemon,d", "Run as a daemon" )
      ("graph-definition-file,f", po::value<string>(), "File containing collaboration graph definition" )
    ( "pid-file", po::value<string>()->default_value( "/var/run/kibitz-locator.pid" ), "Location of pid file for daemon mode" )
      ( "heartbeat-frequency", po::value<int>()->default_value( 100 ), "Heartbeat frequency in milliseconds" )
    ;

    po::variables_map command_line;
    po::store( po::parse_command_line( argc, argv, options ), command_line );
    po::notify( command_line );

    if( command_line.count( "daemon" ) )
    {
        pid_file = command_line["pid-file"].as<string>();
        kibitz::util::daemonize( pid_file );
	// we want to capture SIGINT so we 
	// can clean up pid file and re-raise signal
	struct sigaction act;
	memset( &act, 0, sizeof( act ) );
	act.sa_sigaction = &signal_handler;
	act.sa_flags = SA_SIGINFO | SA_RESETHAND;
	sigaction( SIGINT, &act, NULL );
    }



    InitGoogleLogging( argv[0] );
#ifndef BOOST_WINDOWS
    //This method is not implemented on windows
    InstallFailureSignalHandler();
#endif
    DLOG( INFO ) << "Start locator" ;


    const int port = command_line["port"].as<int>() ;
    const int heartbeat_frequency = command_line["heartbeat-frequency"].as<int>() ;
    int exit_code = 0;
    int rc = 0;

    if( command_line.count( "help" ) )
    {
        std::cout << options << std::endl;
        exit( 1 );
    }

    void* context = zmq_init( command_line["context-threads"].as<int>() );

    if( !context )
    {
        LOG( ERROR ) << "Could not create zmq context " << zmq_errno();
        return 1;
    }

    try
    {
      string publisher_binding = ( format( "tcp://*:%1%" ) % port ).str();
      kg::worker_graph_ptr worker_graph_ptr = kg::create_worker_graph_from_file( command_line["graph-definition-file"].as<string>() );
      kibitz::publisher publisher( context, publisher_binding, ZMQ_PUB, "inproc://publisher" );
      heartbeat_generator heartbeats( publisher, heartbeat_frequency, port );
      binding_broadcaster binder( publisher, worker_graph_ptr );

      boost::thread_group threads;
      threads.create_thread( publisher );
      threads.create_thread( heartbeats );
      threads.create_thread( binder ); 
      threads.join_all();
      DLOG(INFO) << "Exiting";
    }
    catch( const std::exception& ex )
    {
        exit_code = 1;
        LOG( ERROR ) << "An exception killed worker locator " << ex.what() ;
    }

    fs::path path( pid_file );
    if( fs::exists( path ) )
    {
        fs::remove_all( path );
    }


    zmq_term( context );




    return exit_code;

}


