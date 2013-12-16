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
#include <kibitz/kibitz_util.hpp>
#include <kibitz/context.hpp>

#include <kibitz/messages/worker_broadcast_message.hpp>
#include <kibitz/messages/collaboration_message_bundle.hpp>
#include <kibitz/messages/job_initialization_message.hpp>
#include <kibitz/messages/binding_notification.hpp>
#include <kibitz/messages/inproc_notification_message.hpp>
#include <kibitz/collaboration_handler.hpp>

namespace kibitz
{

const char* in_edge_manager::NOTIFICATION_BINDING = "inproc://in_edge_manager";

////////////////////////////////////////////////////////////////////////////////
in_edge_manager::in_edge_manager( context& ctx )
    :
    context_( ctx ),
    worker_type_( ctx.get_config()[ "worker-type" ].as< string >() ),
    worker_id_( ctx.get_config()[ "worker-id" ].as< int >() )
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
in_edge_manager::~in_edge_manager()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
in_edge_manager::in_edge_manager( const in_edge_manager& iem )
    :
    context_( iem.context_ ),
    worker_type_( iem.worker_type_ ),
    worker_id_( iem.worker_id_ )
{
    //We do not initialize notification socket
    ;
}
////////////////////////////////////////////////////////////////////////////////
void in_edge_manager::send_notification( const string& json )
{
    if( notification_socket_ == NULL )
    {
        notification_socket_ =
            util::create_socket_ptr( context_.zmq_context(), ZMQ_REQ );
        util::check_zmq( zmq_connect(
            *notification_socket_, in_edge_manager::NOTIFICATION_BINDING ) );
    }

    util::send( *notification_socket_, json );
    string response;
    util::recv( *notification_socket_, response );
    VLOG( 1 ) << "RESPONSE " << response;
}
////////////////////////////////////////////////////////////////////////////////
void in_edge_manager::check_and_start_job( notification_message_ptr_t message )
{
    DLOG( INFO ) << "Got job initialization message ";
    job_initialization_message_ptr_t job_init_message =
        dynamic_pointer_cast< job_initialization_message >( message );
    CHECK( job_init_message != NULL )
        << "invalid notification message cast to job init message";
    //Only targeted worker will execute init callback,
    //if such callback has been implemented
    if( job_init_message->worker_type() == worker_type_ )
    {
        if( job_init_message->worker_id() == worker_id_ )
        {
            initialization_callback cb =
                context_.get_initialization_notification_callback();
            CHECK( cb != NULL )
                << "Sent a job initialization message to a "
                << "worker without an initialization callback";
	    context_.send_worker_status( WORK_RECIEVED );
            cb( job_init_message->payload() );
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
void in_edge_manager::operator ()()
{
    DLOG( INFO ) << "in edge manager thread started" ;

    try
    {


      util::sockman collab_handler_socket( context_.zmq_context(), ZMQ_PAIR );
      util::check_zmq( zmq_bind( collab_handler_socket, INPROC_COLLABORATION_MESSAGE_HANDLER ) );

      collaboration_handler handler( &context_,  INPROC_COLLABORATION_MESSAGE_HANDLER );
      boost::thread thrd( handler ) ;

        util::sockman_ptr_t notification_sock =
            util::create_socket_ptr( context_.zmq_context(), ZMQ_REP );
        util::check_zmq( zmq_bind(
            *notification_sock, in_edge_manager::NOTIFICATION_BINDING ) );

        zmq_pollitem_t* pollitems =
            ( zmq_pollitem_t* )malloc( sizeof( zmq_pollitem_t ) * 2 );
        int count_items = 1;
        pollitems[ 0 ].socket = *notification_sock;
        pollitems[ 0 ].fd = 0;
        pollitems[ 0 ].events = ZMQ_POLLIN;
        pollitems[ 0 ].revents = 0;
        memset( &pollitems[ 1 ], 0, sizeof( zmq_pollitem_t ) );

        string current_binding;

        while( true )
        {
            int rc = zmq_poll( pollitems, count_items, -1 );
            if( rc > 0 )
            {
                if( pollitems[ 0 ].revents & ZMQ_POLLIN )
                {
                    string json;
                    util::recv( pollitems[ 0 ].socket, json );

                    notification_message_ptr_t msg =
                        static_pointer_cast< notification_message >(
                            message_factory( json ) );

                    inproc_notification_message response( message::ok );
                    util::send( pollitems[ 0 ].socket, response.to_json() );

                    if( msg->notification_type() ==
                        binding_notification::NOTIFICATION_TYPE )
                    {
                        binding_notification_ptr_t bind_msg =
                            static_pointer_cast<binding_notification>( msg ) ;
                        if( bind_msg->target_worker() == worker_type_ )
                        {
                            if( bind_msg->binding() != current_binding )
                            {
                                LOG( INFO )
                                    << "Binding worker to ["
                                    << bind_msg->binding() << "]";
                                if( count_items > 1 )
                                {
                                    util::close_socket( pollitems[ 1 ].socket );
                                }

                                count_items = 2;
                                current_binding = bind_msg->binding();
                                pollitems[ 1 ].socket = util::create_socket(
                                    context_.zmq_context(), ZMQ_PULL );
                                util::check_zmq( zmq_connect(
                                    pollitems[ 1 ].socket,
                                    current_binding.c_str() ) );
                                pollitems[ 1 ].events = ZMQ_POLLIN;
                                DLOG( INFO )
                                    << "Bind operation succeeded to ["
                                    << bind_msg->binding() << "]";
                            }
                        }
                    }
                }

                //Handle collaboration message
                if( pollitems[ 1 ].revents & ZMQ_POLLIN )
                {
                    string json;
                    util::recv( pollitems[ 1 ].socket, json );
                    VLOG( 1 ) << "Received collaboration message " << json;
		    context_.send_worker_status( WORK_RECIEVED );
		    int queue_depth = context_.increment_collaboration_queue();
		    VLOG( 1 ) << (boost::format("Collaboration queue depth is %1%") % queue_depth ).str() ;
		    util::send( collab_handler_socket, json );

                }
            }
            else
            {
                //TODO: handle bad return
            }
        } //end while
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

} //end kibitz
