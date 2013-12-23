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
    worker_type_( ctx.get_config()[ "worker-type" ].as< std::string >() ),
    worker_id_( ctx.get_config()[ "worker-id" ].as< int >() ),
    m_logger( Poco::Logger::get("in_edge_manager") )
{
    m_logStream = LogStreamPtr( new Poco::LogStream( m_logger ) );
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
    worker_id_( iem.worker_id_ ),
    m_logger( Poco::Logger::get("in_edge_manager") )
{
    m_logStream = LogStreamPtr( new Poco::LogStream( m_logger ) );
    //We do not initialize notification socket
}
////////////////////////////////////////////////////////////////////////////////
void in_edge_manager::send_notification( const std::string& json )
{
    if( notification_socket_ == NULL )
    {
        notification_socket_ =
            util::create_socket_ptr( context_.zmq_context(), ZMQ_REQ );
        util::check_zmq( zmq_connect(
                             *notification_socket_, in_edge_manager::NOTIFICATION_BINDING ) );
    }

    util::send( *notification_socket_, json );
    std::string response;
    util::recv( *notification_socket_, response );
    KIBITZ_LOG_DEBUG( "RESPONSE " << response );
}
////////////////////////////////////////////////////////////////////////////////
void in_edge_manager::check_and_start_job( notification_message_ptr_t message )
{
    KIBITZ_LOG_INFO( "Got job initialization message " );
    job_initialization_message_ptr_t job_init_message =
        dynamic_pointer_cast< job_initialization_message >( message );
    poco_assert( job_init_message != NULL );
            //<< "invalid notification message cast to job init message";
    //Only targeted worker will execute init callback,
    //if such callback has been implemented
    if( job_init_message->worker_type() == worker_type_ )
    {
        if( job_init_message->worker_id() == worker_id_ )
        {
            initialization_callback cb =
                context_.get_initialization_notification_callback();
            poco_assert( cb != NULL );
                    //<< "Sent a job initialization message to a "
                    //<< "worker without an initialization callback";
            context_.send_worker_status( WORK_RECIEVED );
            cb( job_init_message->payload() );
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
void in_edge_manager::operator()()
{
    KIBITZ_LOG_INFO( "in edge manager thread started" );

    try
    {


      util::sockman collab_handler_socket( context_.zmq_context(), ZMQ_PAIR );
      util::check_zmq( zmq_bind( collab_handler_socket, INPROC_COLLABORATION_MESSAGE_HANDLER ) );


        util::sockman_ptr_t notification_sock =
            util::create_socket_ptr( context_.zmq_context(), ZMQ_REP );
        util::check_zmq( zmq_bind(
                             *notification_sock, in_edge_manager::NOTIFICATION_BINDING ) );

	util::sockman_ptr_t inedge_sock_ptr ;

        zmq_pollitem_t* pollitems =
            ( zmq_pollitem_t* )malloc( sizeof( zmq_pollitem_t ) * 2 );
        int count_items = 1;
        pollitems[ 0 ].socket = *notification_sock;
        pollitems[ 0 ].fd = 0;
        pollitems[ 0 ].events = ZMQ_POLLIN;
        pollitems[ 0 ].revents = 0;
        memset( &pollitems[ 1 ], 0, sizeof( zmq_pollitem_t ) );

        std::string current_binding;
	const long timeout = 100; // 1/10 second 

        while( true )
        {
            int rc = zmq_poll( pollitems, count_items, timeout );
            if( rc > 0 )
            {
                if( pollitems[ 0 ].revents & ZMQ_POLLIN )
                {

                    std::string json;
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
                                KIBITZ_LOG_NOTICE( "Binding worker to ["
                                        << bind_msg->binding() << "]" );

				// create smart pointer and swap contents with existing, if there is
				// already a socket bound to inedge_sock it will be cleaned up when 
				// we go out of scope
				util::sockman_ptr_t temp_ptr = util::create_socket_ptr( context_.zmq_context(), ZMQ_PULL );
				current_binding = bind_msg->binding();
				util::check_zmq( zmq_connect( *temp_ptr, current_binding.c_str() ) );
				pollitems[1].socket = *temp_ptr;
				pollitems[1].events = ZMQ_POLLIN;
				count_items = 2;
				// swap so old socket gets closed when temp_ptr goes out of scope, 
				// new pointer lives until thread dies then it gets cleaned up
				inedge_sock_ptr.swap( temp_ptr );
                                KIBITZ_LOG_NOTICE( "Bind operation succeeded to ["
                                        << bind_msg->binding() << "]" );
                            }
                        }
                    }
                }

                //Handle collaboration message
                if( pollitems[ 1 ].revents & ZMQ_POLLIN )
                {
                    std::string json;
                    util::recv( pollitems[ 1 ].socket, json );

                    KIBITZ_LOG_DEBUG( "Received collaboration message " << json );
		    context_.send_worker_status( WORK_RECIEVED );
		    int queue_depth = context_.increment_collaboration_queue();
		    KIBITZ_LOG_DEBUG( "Collaboration queue depth is " <<   queue_depth );
		    util::send( collab_handler_socket, json );

                }
            }
            else if( rc == -1 )  
            {
	      int err = zmq_errno();
	      KIBITZ_LOG_WARNING( "An error occurred polling socket in in_edge_manager. Error = " << zmq_strerror(err) );
	      if( errno  == ETERM || errno == EFAULT ) break;
            }
            
            //See if this thread has been interrupted:
            //http://www.boost.org/doc/libs/1_53_0/doc/html/thread/thread_management.html#thread.thread_management.this_thread.interruption_point
            boost::this_thread::interruption_point();
        } //end while
    }
    catch( const util::queue_interrupt& )
    {
        KIBITZ_LOG_NOTICE( "interrupt terminated in edge manager" );
    }
    catch( const std::exception& ex )
    {
        KIBITZ_LOG_ERROR( "exception in edge manager - " << ex.what() );
    }
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz
