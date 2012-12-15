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
#include <kibitz/in_edge_manager.hpp>
#include <kibitz/bus.hpp>
#include <kibitz/kibitz_util.hpp>
#include <kibitz/context.hpp>
#include <kibitz/worker_map.hpp>

#include <kibitz/messages/worker_broadcast_message.hpp>
#include <kibitz/messages/basic_collaboration_message.hpp>
#include <kibitz/messages/job_initialization_message.hpp>
#include <kibitz/messages/binding_notification.hpp>
#include <kibitz/messages/inproc_notification_message.hpp>

namespace kibitz
{

  const char* in_edge_manager::NOTIFICATION_BINDING = "inproc://in_edge_manager";

////////////////////////////////////////////////////////////////////////////////
in_edge_manager::in_edge_manager( context& ctx )
    :
    context_( ctx ),
    worker_type_( ctx.get_config()["worker-type"].as<string>() ),
    worker_id_( ctx.get_config()["worker-id"].as<int>() )
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
in_edge_manager::~in_edge_manager()
{
    ;
}

  in_edge_manager::in_edge_manager( const in_edge_manager& iem ) 
    :context_( iem.context_ ),
     worker_type_( iem.worker_type_ ),
     worker_id_( iem.worker_id_ ) {
    // we do not initialize notification socket

  }

  void in_edge_manager::send_notification( const string& json ) {
    if( notification_socket_ == NULL ) {
      notification_socket_ = util::create_socket_ptr( context_.zmq_context(), ZMQ_REQ );
      util::check_zmq( zmq_connect( *notification_socket_, in_edge_manager::NOTIFICATION_BINDING ) );
    }

    util::send( *notification_socket_, json );
    string response;
    util::recv( *notification_socket_, response );
    VLOG(1) << "RESPONSE " << response;
  }

////////////////////////////////////////////////////////////////////////////////
void in_edge_manager::create_bindings( zmq_pollitem_t** pollitems, int& count_items, int& size_items )
{
    release_bindings( *pollitems, count_items );
    worker_infos_t all_infos;
    worker_map_ptr_t worker_map_ptr = worker_map::get_worker_map( context_.zmq_context() );
    worker_types_t worker_types = context_.get_worker_types();
    BOOST_FOREACH( const string & worker_type, worker_types )
    {
        DLOG( INFO ) << "Resolving workers for " << worker_type ;
        worker_infos_t worker_infos = worker_map_ptr->get_in_edge_workers( worker_type );
        DLOG( INFO ) << "Got " << worker_infos.size() << " hits for " << worker_type;
        all_infos.insert( all_infos.end(), worker_infos.begin(), worker_infos.end() );
    }

    DLOG( INFO ) << "Total in edges = " << all_infos.size() ;
    if( size_items < ( all_infos.size() + 1 ) )
    {
        size_items = all_infos.size() + 1;
        *pollitems = ( zmq_pollitem_t* )realloc( *pollitems, size_items * sizeof( zmq_pollitem_t ) );
    }

    // create bindings for all in edges, in edges
    // start at index 1, index 0 is broadcast notifications
    count_items = all_infos.size() + 1;
    int current = 1;
    BOOST_FOREACH( const worker_info & info, all_infos )
    {
        create_binding( info, ( *pollitems )[current] );
        current++;
    }
}
////////////////////////////////////////////////////////////////////////////////
void in_edge_manager::create_binding( const worker_info& info, zmq_pollitem_t& pollitem )
{
    stringstream stm;
    stm << "tcp://" << info.host << ":" << info.port;
    pollitem.socket = util::create_socket( context_.zmq_context(), ZMQ_SUB );
    pollitem.fd = 0;
    pollitem.events = ZMQ_POLLIN;
    pollitem.revents = 0;
    util::check_zmq( zmq_connect( pollitem.socket, stm.str().c_str() ) );
    util::check_zmq( zmq_setsockopt( pollitem.socket, ZMQ_SUBSCRIBE, "", 0 ) );
}
////////////////////////////////////////////////////////////////////////////////
void in_edge_manager::release_bindings( zmq_pollitem_t* pollitems, int count_items )
{
    // don't close item zero socket, its managed by broadcast_subscriber
    for( int item = 1; item < count_items; ++item )
    {
        util::close_socket( pollitems[item].socket );
    }
}

void in_edge_manager::create_collaboration_binding( notification_context_t& context, notification_message_ptr_t message ) {


}

////////////////////////////////////////////////////////////////////////////////
void in_edge_manager::handle_notification_message( notification_context_t& context ) {

    if( ( context.pollitems[0].revents | ZMQ_POLLIN ) == ZMQ_POLLIN )
    {
        string json;
        util::recv( context.pollitems[0].socket, json );
        VLOG(1) << "notification handler got " << json ;
	inproc_notification_message response( message::ok );
	util::send( context.pollitems[0].socket, response.to_json() );

        notification_message_ptr_t notification_message_ptr = dynamic_pointer_cast<notification_message>( message_factory( json ) );
        string notification_type = notification_message_ptr->notification_type() ;

        if( notification_type == notification::JOB_INITIALIZATION )
        {
            check_and_start_job( notification_message_ptr );
        } else if( notification_type == binding_notification::NOTIFICATION_TYPE ) {
	  create_collaboration_binding( context, notification_message_ptr );
	}	
        else
        {
            LOG( WARNING ) << "in edge manager get a message that it doesn't understand - " << json ;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
void in_edge_manager::check_and_start_job( notification_message_ptr_t message )
{
    DLOG( INFO ) << "Got job initialization message ";
    job_initialization_message_ptr_t job_init_message = dynamic_pointer_cast<job_initialization_message>( message );
    CHECK( job_init_message != NULL ) << "invalid notification message cast to job init message";
    // only targeted worker will execute init callback, if such callback has been implemented
    if( job_init_message->worker_type() == worker_type_ )
    {
        if( job_init_message->worker_id() == worker_id_ )
        {
            initialization_callback cb = context_.get_initialization_notification_callback();
            CHECK( cb != NULL ) << "Sent a job initialization message to a worker without an initialization callback";
            cb(job_init_message->payload());
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
bool in_edge_manager::all_messages_arrived( const string& job_id, collaboration_context_t& collab_context ) const
{
    return collab_context.job_messages[job_id].size() >= ( collab_context.count_items - 1 );
}
////////////////////////////////////////////////////////////////////////////////
void in_edge_manager::handle_collaboration_message( collaboration_context_t& context )
{
    for( int item = 1; item < context.count_items; ++item )
    {
        if( ( context.pollitems[item].revents | ZMQ_POLLIN ) == ZMQ_POLLIN )
        {
            string json ;
            util::recv( context.pollitems[item].socket, json );
            LOG( INFO ) << "Got collaboration message -> " << json ;
            collaboration_message_ptr_t collab_message = dynamic_pointer_cast<collaboration_message>( message_factory( json ) );

            if( collab_message == NULL )
            {
                LOG( WARNING ) << "don't know how to handle collaboration message " << json;
                return;
            }

            string collaboration_type = collab_message->collaboration_type();

            if( collaboration_type == "generic" )
            {
                basic_collaboration_message_ptr_t basic_collaboration = static_pointer_cast<basic_collaboration_message>( collab_message ) ;
                string job_id = basic_collaboration->job_id() ;
                string worker_type = basic_collaboration->worker_type();
                DLOG( INFO ) << "worker " << context_.worker_type() << ":" << context_.worker_id()  << " got a message from " << worker_type << " job " << job_id;
                context.job_messages[job_id][worker_type] =  basic_collaboration->payload() ;
                if( all_messages_arrived( job_id, context ) )
                {
                    collaboration_callback cbfn = context_.get_inedge_message_handler();
                    if( cbfn )
                    {
                        // hang on to job id so we can attach it to send messages
                        context_.set_job_id( job_id );
                        cbfn( context.get_job_messages( job_id ) );
                    }
                    else
                    {
                        LOG( WARNING ) << "Got callaboration messages, no callback defined to handle messages";
                    }
                    context.job_messages.erase( job_id );
                }
            }
            else
            {
                LOG( WARNING ) << "don't know how to handle collaboration type " << collaboration_type;
            }
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
void in_edge_manager::operator()()
{
    DLOG( INFO ) << "in edge manager thread started" ;

    try
    {
	util::sockman_ptr_t notification_sock = util::create_socket_ptr( context_.zmq_context(), ZMQ_REP );
	util::check_zmq( zmq_bind( *notification_sock, in_edge_manager::NOTIFICATION_BINDING ) );

        zmq_pollitem_t* pollitems = ( zmq_pollitem_t* )malloc( sizeof( zmq_pollitem_t ) * 2 );
        int count_items = 1;
        pollitems[0].socket = *notification_sock;
        pollitems[0].fd = 0;
        pollitems[0].events = ZMQ_POLLIN;
	pollitems[0].revents = 0;
	memset( &pollitems[1], 0, sizeof( zmq_pollitem_t ) );

	string current_binding;

        while( true )
        {
            int rc = zmq_poll( pollitems, count_items, -1 );
            if( rc > 0 )
            {
	      if( pollitems[0].revents & ZMQ_POLLIN ) { 
		string json;
		util::recv( pollitems[0].socket, json );

		notification_message_ptr_t msg = static_pointer_cast<notification_message>(message_factory( json ) );

		inproc_notification_message response( message::ok );
		util::send( pollitems[0].socket, response.to_json() );

		if( msg->notification_type() == binding_notification::NOTIFICATION_TYPE ) {
		  binding_notification_ptr_t bind_msg = static_pointer_cast<binding_notification>( msg ) ;
		  if( bind_msg->target_worker() == worker_type_ ) {
		    if( bind_msg->binding() != current_binding ) {
		      LOG(INFO) << "Binding worker to [" << bind_msg->binding() << "]";
		      if( count_items > 1 ) {
			util::close_socket(  pollitems[1].socket );
		      }

		      count_items = 2;
		      current_binding = bind_msg->binding();
		      pollitems[1].socket = util::create_socket( context_.zmq_context(), ZMQ_PULL );
		      util::check_zmq( zmq_connect( pollitems[1].socket, current_binding.c_str() ) );
		      pollitems[1].events = ZMQ_POLLIN;
		      DLOG(INFO) << "Bind operation succeeded to [" << bind_msg->binding() << "]";
		    }
		  }		  
		}
		
	      }

	      //	      handle_notification_message( thread_context ) ; 
		//                collab_context.count_items = count_items;
                // collab_context.pollitems = pollitems;
                // handle_collaboration_message( collab_context );
            }
        }
    }
    catch( const util::queue_interrupt& )
    {
        DLOG( INFO ) << "interrupt terminated in edge manager" ;
    }
    catch( const std::exception& ex )
    {
        LOG( ERROR ) << "exception nuked in edge manager - " << ex.what() ;
    }
}
////////////////////////////////////////////////////////////////////////////////
}
