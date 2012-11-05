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
#include <kibitz/context.hpp>
#include <kibitz/heartbeat_sender.hpp>
#include <kibitz/heartbeat_receiver.hpp>
#include <kibitz/kibitz_util.hpp>
#include <kibitz/worker_map.hpp>
#include <yaml-cpp/yaml.h>
#include <kibitz/in_edge_manager.hpp>
#include <kibitz/messages/basic_collaboration_message.hpp>

namespace kibitz
{


context::context( const po::variables_map& application_configuration )
    : application_configuration_( application_configuration ),
      zmq_context_( NULL ),
      signalled_( false ),
      inedge_message_handler_( NULL ),
      initialization_handler_( NULL )
{

    DLOG( INFO ) << "ctor for context entered" ;
    zmq_context_ = zmq_init( application_configuration["context-threads"].as<int>() );
    DLOG( INFO ) << "zmq initialized";
    message_bus_socket_ = util::create_socket( zmq_context_, ZMQ_PUB );
    util::check_zmq( zmq_bind( message_bus_socket_, INPROC_COMMAND_BINDING ) );
    stringstream stm1, stm2;
    stm1 << "tcp://*:" << application_configuration["publish-port"].as<int>();
    stm2 << "tcp://*:" << application_configuration["notification-port"].as<int>();

    collaboration_publisher_ptr_ = shared_ptr<pub>( new pub( zmq_context_, stm1.str().c_str() ) );

    notification_publisher_ptr_ = shared_ptr<pub>( new pub( zmq_context_, stm2.str().c_str() ) );
}

context::~context()
{
}

void context::send_out_message( const string& payload )
{
    string worker_type = application_configuration_["worker-type"].as<string>();
    string job_id;
    get_job_id( job_id );

    basic_collaboration_message msg( worker_type, payload );
    if( !job_id.empty() )
    {
        msg.set_job_id( job_id ) ;
    }


    collaboration_publisher_ptr_->send( msg.to_json() );

}

void context::send_notification_message( const string& payload )
{
    notification_publisher_ptr_->send( payload );
}

void context::register_initialization_notification_handler( initialization_callback initialization_handler )
{
    initialization_handler_ = initialization_handler;
}

initialization_callback context::get_initialization_notification_callback()
{
    return initialization_handler_;
}

void context::register_inedge_message_handler( collaboration_callback inedge_message_handler )
{
    inedge_message_handler_ = inedge_message_handler;
}

collaboration_callback context::get_inedge_message_handler()
{
    return inedge_message_handler_;
}

const po::variables_map& context::get_config() const
{
    return application_configuration_;
}

void context::send_internal_message( const char* message )
{
    string s( message );
    DLOG( INFO ) << "sending internal message -> " << message;
    kibitz::util::send( message_bus_socket_, s );
}

void context::set_worker_type( const string& worker_type_name )
{
    worker_type_name_ = worker_type_name;
}

void context::set_worker_id( const string& worker_id )
{
    worker_id_ = worker_id ;
}



void context::start()
{
    heartbeat_sender hb_sender( this );
    heartbeat_receiver hb_receiver( this );
    worker_map wmap( this );
    kibitz::in_edge_manager in_edge_manager( *this );

    threads_.create_thread( wmap );
    threads_.create_thread( hb_sender );
    threads_.create_thread( hb_receiver );
    threads_.create_thread( in_edge_manager );


    threads_.join_all();

}



void context::stop()
{
}

void context::terminate()
{
    DLOG( INFO ) << "context.terminate shutting down application" ;
    collaboration_publisher_ptr_->close();
    notification_publisher_ptr_->close();

    util::close_socket( message_bus_socket_ );

    if( zmq_context_ )
    {
        zmq_term( zmq_context_ );
    }
}

void* context::zmq_context()
{
    return zmq_context_;
}


void context::set_job_id( const string& job_id )
{
    boost::mutex::scoped_lock lock( mutex_ ) ;
    current_job_id_ = job_id;
}

void context::get_job_id( string& job_id )
{
    boost::mutex::scoped_lock lock( mutex_ );
    job_id = current_job_id_;
}

}

