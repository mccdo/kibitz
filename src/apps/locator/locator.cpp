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

#include "locator.hpp"

#include <kibitz/logging.hpp>

#include <kibitz/locator/binding_broadcaster.hpp>
#include <kibitz/locator/heartbeat_generator.hpp>
#include <kibitz/locator/router.hpp>

#include <kibitz/messages/heartbeat.hpp>

#include <Poco/Logger.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FileChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>

#include <iostream>
#include <string>
#include <assert.h>
#include <signal.h>

#include <boost/config.hpp>
#ifdef BOOST_WINDOWS
# pragma warning(disable: 4275)
#else
#include <kibitz/GNUCompilerGuards.hpp>
GCC_DIAG_OFF( unused-parameter )
#endif

#include <boost/program_options.hpp>

#ifdef BOOST_WINDOWS
# pragma warning(default: 4275)
#else
GCC_DIAG_ON( unused-parameter )
#endif

#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
using boost::format;
namespace po = boost::program_options;

using kibitz::util::create_socket;
using kibitz::util::check_zmq;
using kibitz::util::close_socket;

using kibitz::locator::heartbeat_generator;
using kibitz::locator::binding_broadcaster;
using kibitz::locator::binding_map_t;
namespace kg = kibitz::graph;
namespace kl = kibitz::locator;

std::string pid_file;
#ifndef BOOST_WINDOWS
void signal_handler( int, siginfo_t*, void* )
{
    fs::path path( pid_file );
    if( fs::exists( path ) )
    {
        fs::remove_all( path );
    }
    raise( SIGINT );
}
#endif
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char* argv[] )
{
  // grab this first thing as it will change after daemonization
  std::string default_logfile = "locator.log";
  boost::filesystem::path  current_working_directory = boost::filesystem::current_path() ;
  boost::filesystem::path log_path = current_working_directory / default_logfile;
 
    po::options_description options( "locator" );
    options.add_options()
    ( "help,h",
      "Show help message" )
    ( "port,p",         po::value< int >()->default_value( 5556 ),
      "Port used by locator to distribute heartbeats." )
    ( "listen-port,l",  po::value< int >()->default_value( 5557 ),
      "Port listens for messages from workers and control scripts." )
    ( "context-threads,t", po::value< int >()->default_value( 4 ),
      "zmq context thread count" )
    ( "daemon,d", "Run as a daemon" )
    ( "graph-definition-file,f", po::value< std::string >(),
      "File containing collaboration graph definition" )
    ( "pid-file",       po::value< std::string >()->default_value( "/var/run/kibitz-locator.pid" ),
      "Location of pid file for daemon mode" )
    ( "heartbeat-frequency", po::value< int >()->default_value( 100 ),
      "Heartbeat frequency in milliseconds" )
    ( "host,H",         po::value< std::string >(),
      "Host name or IP address that workers will connect to for collaboration messages." )
    ( "base-port,P",    po::value< int >()->default_value( 6000 ),
      "Start of port range that workers will bind to for collaboration messages" )
    ( "log-level,o",    po::value< std::string >()->default_value("warning"),
      "Set logging level -- fatal, critical, error, warning, notice, information, debug, trace")
      ( "log-path", po::value< std::string >()->default_value(log_path.string()), 
	(boost::format("Path to the application log file, defaults to %1%") % log_path.string()).str().c_str() );

    po::variables_map command_line;
    po::store( po::parse_command_line( argc, argv, options ), command_line );
    po::notify( command_line );

#ifndef BOOST_WINDOWS
    if( command_line.count( "daemon" ) )
    {
        pid_file = command_line[ "pid-file" ].as< std::string >();
        kibitz::util::daemonize( pid_file );
        // we want to capture SIGINT so we
        // can clean up pid file and re-raise signal
        struct sigaction act;
        memset( &act, 0, sizeof( act ) );
        act.sa_sigaction = &signal_handler;
        act.sa_flags = SA_SIGINFO | SA_RESETHAND;
        sigaction( SIGINT, &act, NULL );
    }
#endif

    ///////////////////////////////////
    // Set up a logger to push messages out to a file.
    Poco::Logger& rootLogger( Poco::Logger::get( "" ) );
    std::string logLevel = command_line["log-level"].as<std::string>();
    rootLogger.setLevel( logLevel );
    Poco::FileChannel* fileChannel = new Poco::FileChannel;

    log_path = command_line["log-path"].as<std::string>();
    
    
    //    if( !boost::filesystem::exists( log_path ) )
    // {
    //   boost::filesystem::create_directory( log_path );
    // }
    
    
    std::cout << "LogPath = " << log_path.string() << std::endl;
    
    fileChannel->setProperty( "path", log_path.string() );
    fileChannel->setProperty( "rotation", "1M" );
    fileChannel->setProperty( "archive", "number" );
    fileChannel->setProperty( "purgeCount", "5" );
    
    // Format the logged output as
    // time_with_microseconds [thread number] (priority) source message extra_crlf
    Poco::PatternFormatter* formatter = new Poco::PatternFormatter;
    formatter->setProperty( "pattern", "%H:%M:%S:%F [%I] (%l) %s: %t" );
    formatter->setProperty( "times", "local" );
    Poco::FormattingChannel* formattingChannel = new Poco::FormattingChannel( formatter , fileChannel );
    
    rootLogger.setChannel( formattingChannel );
    rootLogger.fatal( "#####################################################" );
    rootLogger.fatal( "locator started." );
    ///////////////////////////////////

    KIBITZ_STATIC_LOG_NOTICE( "locator", "Start locator" );

    const int port = command_line[ "port" ].as< int >();
    const int heartbeat_frequency =
        command_line[ "heartbeat-frequency" ].as< int >();
    int exit_code = 0;
    int rc = 0;

    if( command_line.count( "help" ) )
    {
        std::cout << options << std::endl;
        exit( 1 );
    }

    void* context = zmq_init( command_line[ "context-threads" ].as< int >() );

    if( !context )
    {
        KIBITZ_STATIC_LOG_ERROR( "locator", "Could not create zmq context " << zmq_errno() );
        return 1;
    }

    try
    {
        KIBITZ_STATIC_LOG_NOTICE( "locator", "Beginning initialization" );
        std::string publisher_binding = ( boost::format( "tcp://*:%1%" ) % port ).str();
        std::string worker_root_binding = ( boost::format( "tcp://%1%" ) %
                                       command_line[ "host" ].as< std::string >() ).str();
        std::string listener_binding = ( boost::format( "tcp://*:%1%" ) %
                                    command_line[ "listen-port" ].as< int >() ).str();
        std::string graph_file_name =
            command_line[ "graph-definition-file" ].as< std::string >() ;
        KIBITZ_STATIC_LOG_NOTICE( "locator", "Preparing collaboration graph from " << graph_file_name );
        kg::worker_graph_ptr worker_graph_ptr =
            kg::create_worker_graph_from_file( graph_file_name );
        KIBITZ_STATIC_LOG_NOTICE( "locator", "Graph creation succeeded" );
        kibitz::publisher pub(
            context,
            publisher_binding,
            ZMQ_PUB,
            "inproc://publisher",
            kibitz::publish::bind );
        heartbeat_generator heartbeats( pub, heartbeat_frequency, port );

        kl::binding_map_t bindings;
        kl::create_bindings( worker_root_binding, worker_graph_ptr,
                             command_line[ "base-port" ].as< int >(), bindings );

        kl::router router(
            context, pub, listener_binding, bindings, worker_graph_ptr ) ;
        binding_broadcaster binder( pub, bindings );

        KIBITZ_STATIC_LOG_NOTICE( "locator", "Creating worker threads" );
        boost::thread_group threads;
        threads.create_thread( pub );
        KIBITZ_STATIC_LOG_NOTICE( "locator", "Created notification message publisher" );
        threads.create_thread( heartbeats );
        KIBITZ_STATIC_LOG_NOTICE( "locator", "Created heartbeat generator" );
        threads.create_thread( router );
        KIBITZ_STATIC_LOG_NOTICE( "locator", "Created message router" );
        threads.create_thread( binder );
        KIBITZ_STATIC_LOG_NOTICE( "locator", "Ready... Initialization complete" );
        threads.join_all();
        KIBITZ_STATIC_LOG_NOTICE( "locator", "Exiting" );
    }
    catch( const std::exception& ex )
    {
        exit_code = 1;
        KIBITZ_STATIC_LOG_ERROR( "locator", "An exception killed worker locator " << ex.what() );
    }

    fs::path path( pid_file );
    if( fs::exists( path ) )
    {
        fs::remove_all( path );
    }

    zmq_term( context );

    return exit_code;
}
////////////////////////////////////////////////////////////////////////////////
