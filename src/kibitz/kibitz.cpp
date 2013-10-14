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
////////////////////////////////////////////////////////////////////////////////
boost::program_options::options_description get_command_line_description()
{
    boost::program_options::options_description generalOptionDesc( "Kibitz worker command line options" );
    generalOptionDesc.add_options()
        ( "help,h", "Show help message" )
        ( "worker-id,I",            po::value< int >(), "(Required) Integer that identifies worker. Must be unique with worker type" )
        ( "worker-type,T",          po::value< string >(), "(Required) Name of the type of worker." )
        //TODO: implement HA pair host1;host2
        ( "locator-host,L",         po::value< string >(), "IP Address or DNS name of locator" )
        ( "locator-receive-port,R", po::value< int >()->default_value( 5556 ), "Port to receive notifications from locator" )
        ( "locator-send-port,S",    po::value< int >()->default_value( 5557 ), "Port to send messages to locator" )
        ( "notification-port,P",    po::value< int >(), "Optional port to publish notification messages" )
        ( "context-threads,t",      po::value< int >()->default_value( 2 ), "Thread count passed to zmq_init" )
        ( "status-sink-binding,b",  po::value< string >(), "Optional zmq binding for listener for status messages such as service start and stop" );
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
    DLOG( INFO ) << "initialize start";
    
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
    assert( !context_ );

    google::InitGoogleLogging( "kibitz_worker" );
#ifndef BOOST_WINDOWS
    //On windows this is not implemented
    google::InstallFailureSignalHandler();
#endif

    po::notify( command_line );

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
////////////////////////////////////////////////////////////////////////////////
void start()
{
    assert( context_ );
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
