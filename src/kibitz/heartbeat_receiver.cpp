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
#include <kibitz/heartbeat_receiver.hpp>
#include <kibitz/kibitz_util.hpp>
#include <kibitz/messages/heartbeat.hpp>
#include <kibitz/messages/worker_broadcast_message.hpp>
#include <kibitz/messages/job_initialization_message.hpp>
#include <kibitz/in_edge_manager.hpp>
#include <kibitz/messages/binding_notification.hpp>
#include <kibitz/initialization_handler.hpp>

using kibitz::util::create_socket;
using kibitz::util::check_zmq;

namespace kibitz
{

////////////////////////////////////////////////////////////////////////////////
heartbeat_receiver::heartbeat_receiver( context* context )
    :
    message_base( context ),
    m_logger( Poco::Logger::get("heartbeat_receiver") )
{
    m_logStream = LogStreamPtr( new Poco::LogStream( m_logger ) );
}
////////////////////////////////////////////////////////////////////////////////
heartbeat_receiver::~heartbeat_receiver()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
void heartbeat_receiver::operator()()
{
    KIBITZ_LOG_NOTICE( "Entered heartbeat receiver thread." );

    try
    {
        util::sockman_ptr_t listen_sock =
            util::create_socket_ptr( context_->zmq_context(), ZMQ_SUB );
        //TODO: handle host of the form primary;secondary, separate
        //primary;secondary, hang on to secondary host and try to use it.
        //Probably listen simulateously to both primary and secondary,
        //when secondary gets flagged as primary use that
        std::string binding = ( boost::format( "tcp://%1%:%2%" ) %
                           context_->get_config()[ "locator-host" ].as< std::string >() %
                           context_->get_config()[ "locator-receive-port" ].as< int >() ).str();

        KIBITZ_LOG_NOTICE( "Will subscribe to messages from locator on " << binding );
        check_zmq( zmq_connect( *listen_sock, binding.c_str() ) );
        check_zmq( zmq_setsockopt( *listen_sock, ZMQ_SUBSCRIBE, "", 0 ) );

        util::wait( util::STARTUP_PAUSE );
        in_edge_manager in_edges( *context_ );

        while( true )
        {
            KIBITZ_LOG_INFO( "Waiting for heartbeat" );
            std::string json;
            //TODO if we dont receive a heartbeat after a certain amount of time
            //try to connect to alternative locator
            kibitz::util::recv( *listen_sock, json );
            KIBITZ_LOG_INFO( context_->worker_type() << ":"
                    << context_->worker_id() << " received message " << json );
            notification_message_ptr_t message_ptr =
                dynamic_pointer_cast< notification_message >(
                    message_factory( json ) );
            if( message_ptr != NULL )
            {
                std::string notification_type = message_ptr->notification_type();
                if( notification_type == "heartbeat" )
                {
                    KIBITZ_LOG_DEBUG( "Got heartbeat" );
                }
                else if( notification_type ==
                         binding_notification::NOTIFICATION_TYPE )
                {
                    KIBITZ_LOG_DEBUG( "Got binding message" );
                    in_edges.send_notification( json );
                }
                else if( notification_type == "worker_broadcast" )
                {
                    worker_broadcast_message_ptr_t wb =
                        dynamic_pointer_cast< worker_broadcast_message >(
                            message_ptr ) ;
                    if( wb->notification() == "shutdown" )
                    {
                        KIBITZ_LOG_DEBUG( "Received shutdown message" );
                        break;
                    }
                }
                else if( notification_type ==
                         job_initialization_message::NOTIFICATION_TYPE )
                {
                    job_initialization_message_ptr_t jm =
                        static_pointer_cast< job_initialization_message >(
                            message_ptr ) ;
                    if( jm->worker_type() == context_->worker_type() )
                    {
                        if( jm->worker_id() == context_->worker_id() )
                        {
                            if( context_->get_initialization_notification_callback() )
                            {
                                boost::thread thrd( initialization_handler(
                                                        context_, jm->payload() ) );
                            }
                            else
                            {
                                KIBITZ_LOG_ERROR( "There is not a callback function "
                                        << "defined to handle the received job "
                                        << "initialization message." );
                            }
                        }
                    }
                }
                else
                {
                    KIBITZ_LOG_ERROR( "We got a message we don't know how to handle - "
                            << json );
                }
            }
            else
            {
                KIBITZ_LOG_ERROR( "Unknown message type. Raw message = " << json );
            }
            //See if this thread has been interrupted:
            //http://www.boost.org/doc/libs/1_53_0/doc/html/thread/thread_management.html#thread.thread_management.this_thread.interruption_point
            boost::this_thread::interruption_point();
        }
    }
    catch( const util::queue_interrupt& )
    {
        KIBITZ_LOG_NOTICE( "Received interrupt shutting down heartbeat receiver thread" );
    }
    catch( const std::exception& ex )
    {
        KIBITZ_LOG_ERROR( "Exception caused thread to terminate " << ex.what() );
    }
}
////////////////////////////////////////////////////////////////////////////////

}//end kibitz
