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
//#include "heartbeat_sender.hpp"
#include <signal.h>


namespace kibitz
{
context* context_ =  NULL;







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

void initialize( int argc, char* argv[] )
{
    assert( !context_ );

    google::InitGoogleLogging( argv[0] );
#ifndef BOOST_WINDOWS
    //On windows this is not implemented
    google::InstallFailureSignalHandler();
#endif

    DLOG( INFO ) << "initialize start";
    fs::path path( argv[0] );
    stringstream ss ;
    ss << path.filename() << " options" ;
    po::options_description options( ss.str() );
    options.add_options()
    ( "help,h", "Show help message" )
    ( "worker-id,I", po::value< int >(), "(Required) Integer that identifies worker. Must be unique with worker type" )
    ( "worker-type,T", po::value< string >(), "(Required) Name of the type of worker." )
      ( "locator-host,L", po::value< string >(), "IP Address or DNS name of locator" )     /* todo: implement HA pair host1;host2 */
      ( "locator-receive-port,R", po::value< int >()->default_value(5556), "Port to receive notifications from locator" )
      ( "locator-send-port,S", po::value< int >()->default_value(5557), "Port to send messages to locator" )
    ( "notification-port,P", po::value<int>()->default_value( 9999 ), "Port to publish notification messages" )
    ( "context-threads,t", po::value< int >()->default_value( 2 ), "Thread count passed to zmq_init" ) 
      ;

    po::variables_map command_line;
    po::store( po::parse_command_line( argc, argv, options ), command_line );
    po::notify( command_line );

    if( command_line.count( "help" ) )
    {
        std::cout << options << std::endl;
        exit( 1 );
    }

    validate_command_line( command_line );

    try
    {
        context_ =  new context( command_line ) ;
    }
    catch( std::exception& e )
    {
        std::cout << "Failed to create zmq context: "
                  << zmq_strerror( errno ) << std::endl;
        throw;
    }
}

void start()
{
    assert( context_ );
    context_->start();
}

void terminate()
{
    if( context_ )
    {
        context_->terminate();
    }
}

void set_in_message_handler( collaboration_callback fn )
{
    context_->register_inedge_message_handler( fn );
}

void set_initialization_notification_handler( initialization_callback fn )
{
    context_->register_initialization_notification_handler( fn ) ;
}



void send_out_message( const string& payload )
{
    context_->send_out_message( payload );
}

void send_notification_message( const string& payload )
{
    context_->send_notification_message( payload );
}


void get_context_information( context_information_t& context_information )
{
    context_information.worker_type = context_->worker_type();
    context_information.worker_id = context_->worker_id();
    context_->get_job_id( context_information.job_id );
}

}

