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

#include <kibitz/kibitz.hpp>
#include <kibitz/common.hpp>
#include <kibitz/context.hpp>

#include <Poco/Logger.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FileChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>

namespace kibitz
{
context* context_ = NULL;
////////////////////////////////////////////////////////////////////////////////
boost::program_options::options_description get_command_line_description()
{
    boost::program_options::options_description generalOptionDesc( "Kibitz worker command line options" );
    generalOptionDesc.add_options()
    ( "help,h", "Show help message" )
    ( "worker-id,I",            po::value< int >(), "(Required) Integer that identifies worker. Must be unique with worker type" )
    ( "worker-type,T",          po::value< std::string >(), "(Required) Name of the type of worker." )
    ( "log-level,o",             po::value< std::string >()->default_value("warning"),"Set logging level -- fatal, critical, error, warning, notice, information, debug, trace")
    //TODO: implement HA pair host1;host2
    ( "locator-host,L",         po::value< std::string >(), "IP Address or DNS name of locator" )
    ( "locator-receive-port,R", po::value< int >()->default_value( 5556 ), "Port to receive notifications from locator" )
    ( "locator-send-port,S",    po::value< int >()->default_value( 5557 ), "Port to send messages to locator" )
    ( "notification-port,P",    po::value< int >(), "Optional port to publish notification messages" )
    ( "context-threads,t",      po::value< int >()->default_value( 2 ), "Thread count passed to zmq_init" )
    ( "status-sink-binding,b",  po::value< std::string >(), "Optional zmq binding for listener for status messages such as service start and stop" );
    return generalOptionDesc;
}
////////////////////////////////////////////////////////////////////////////////
void validate_command_line( const po::variables_map& command_line )
{
    if( !command_line.count( "worker-id" ) )
    {
        throw runtime_error( string( "Missing required worker-id argument" ) );
    }

    if( !command_line.count( "worker-type" ) )
    {
        throw runtime_error( string( "Missing required worker-type argument" ) );
    }
}
////////////////////////////////////////////////////////////////////////////////
void initialize( int argc, char* argv[] )
{
    KIBITZ_STATIC_LOG_NOTICE( "kibitz::initialize", "initialize start" );

    po::variables_map command_line;
    po::store( po::parse_command_line( argc, argv, get_command_line_description() ), command_line );

    if( command_line.count( "help" ) )
    {
        std::cout << get_command_line_description() << std::endl;
        return;
    }

    initialize( command_line );
}
////////////////////////////////////////////////////////////////////////////////
void initialize( po::variables_map& command_line )
{
    if( context_ )
    {
        KIBITZ_STATIC_LOG_NOTICE( "kibitz::initialize", "context has already been initialized" );
        return;
    }

    po::notify( command_line );
    
    validate_command_line( command_line );

    ///////////////////////////////////
    // Set up a logger to push messages out to a file.
    Poco::Logger& rootLogger( Poco::Logger::get( "" ) );
    std::string logLevel = command_line["log-level"].as<std::string>();
    rootLogger.setLevel( logLevel );
    Poco::FileChannel* fileChannel = new Poco::FileChannel;
    std::string logPath;
    
    logPath = ".";
    logPath.append( "/" );
    
    //boost::filesystem::path p( logPath );
    //if( !boost::filesystem::exists( p ) )
    //{
    //    boost::filesystem::create_directory( p );
    //}
    
    const std::string workerType =
        command_line["worker-type"].as<std::string>();
    const std::string workerId =
        boost::lexical_cast< std::string >( command_line["worker-id"].as<int>() );
    
    logPath += "kibitz_" + workerId + "_" + workerType + ".log";
    std::cout << "LogPath = " << logPath << std::endl;
    
    fileChannel->setProperty( "path", logPath );
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
    rootLogger.fatal( "kibitz started." );
    ///////////////////////////////////


    try
    {
        context_ =  new context( command_line ) ;
    }
    catch( std::exception& e )
    {
        KIBITZ_STATIC_LOG_ERROR( "kibitz::initialize", "Failed to create zmq context: "
                  << zmq_strerror( errno ) << std::endl );
        throw runtime_error( std::string( "Unable to create kibitz context" ) );
    }
}
////////////////////////////////////////////////////////////////////////////////
void start()
{
    if( !context_ )
    {
        KIBITZ_STATIC_LOG_NOTICE( "kibitz::start", "kibitz::initialized must be called before you can start the worker process" );
        return;
    }

    context_->start();
}
////////////////////////////////////////////////////////////////////////////////
void terminate()
{
    if( context_ )
    {
        context_->terminate();
    }
}
////////////////////////////////////////////////////////////////////////////////
void set_in_message_handler( collaboration_callback fn )
{
    context_->register_inedge_message_handler( fn );
}
////////////////////////////////////////////////////////////////////////////////
void set_initialization_notification_handler( initialization_callback fn )
{
    context_->register_initialization_notification_handler( fn ) ;
}
////////////////////////////////////////////////////////////////////////////////
void send_out_message( const string& payload )
{
    context_->send_out_message( payload );
}
////////////////////////////////////////////////////////////////////////////////
void send_notification_message( const string& payload )
{
    context_->send_notification_message( payload );
}
////////////////////////////////////////////////////////////////////////////////
void get_context_information( context_information_t& context_information )
{
    context_information.worker_type = context_->worker_type();
    context_information.worker_id = context_->worker_id();
    context_->get_job_id( context_information.job_id );
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz
