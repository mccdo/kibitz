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
#include <kibitz/heartbeat_receiver.hpp>
#include <kibitz/kibitz_util.hpp>
#include <kibitz/messages/worker_notification.hpp>
#include <kibitz/in_edge_manager.hpp>
#include <kibitz/collaboration_handler.hpp>
#include <kibitz/publisher.hpp>
#include <kibitz/messages/basic_collaboration_message.hpp>

namespace kibitz
{

////////////////////////////////////////////////////////////////////////////////
context::context( const po::variables_map& application_configuration )
    :
    application_configuration_( application_configuration ),
    zmq_context_( NULL ),
    signalled_( false ),
    inedge_message_handler_( NULL ),
    initialization_handler_( NULL ),
    status_publisher_enabled_(false),
    collaboration_queue_depth_(0),
    m_logger( Poco::Logger::get("context") )
{
    m_logStream = LogStreamPtr( new Poco::LogStream( m_logger ) );

    KIBITZ_LOG_NOTICE( "ctor for context entered" );
    zmq_context_ = zmq_init(
                       application_configuration[ "context-threads" ].as< int >() );
    KIBITZ_LOG_NOTICE( "zmq initialized" );
    message_bus_socket_ = util::create_socket( zmq_context_, ZMQ_PUB );
    util::check_zmq( zmq_bind( message_bus_socket_, INPROC_COMMAND_BINDING ) );
}
////////////////////////////////////////////////////////////////////////////////
context::~context()
{
    ;
}



  int context::increment_collaboration_queue() {
    int current_count = 0;
    {
      boost::mutex::scoped_lock lock( collaboration_queue_lock_ );
      current_count  = ++collaboration_queue_depth_;
    }
    return current_count;
  }

  int context::decrement_collaboration_queue() {
    int current_count = 0;
    {
      boost::mutex::scoped_lock lock( collaboration_queue_lock_ );
      current_count  = --collaboration_queue_depth_;
    }
    return current_count;
  }

  int context::get_collaboration_queue_size()   {
    int current_count = 0;
    {
      boost::mutex::scoped_lock lock( collaboration_queue_lock_ );
      current_count  = collaboration_queue_depth_;
    }
    return current_count;
  }
////////////////////////////////////////////////////////////////////////////////
void context::send_out_message( const std::string& payload )
{
    std::string worker_type =
        application_configuration_[ "worker-type" ].as< std::string >();
    std::string job_id;
    get_job_id( job_id );

    basic_collaboration_message msg( worker_type, payload );
    if( !job_id.empty() )
    {
        msg.set_job_id( job_id ) ;
    }

    publisher pub( zmq_context(), INPROC_LOCATOR_PUBLISH_BINDING );
    pub.send( msg.to_json() );
    send_worker_status( WORK_SENT );
}
////////////////////////////////////////////////////////////////////////////////
void context::send_notification_message( const std::string& payload )
{
    if( application_configuration_.count( "notification-port" ) )
    {
        worker_notification notification( payload ) ;
        KIBITZ_LOG_DEBUG( "Published notification " << payload );
        publisher pub( zmq_context(), INPROC_NOTIFICATION_PUBLISH_BINDING );
        pub.send( notification.to_json() ) ;
    }
    else
    {
        KIBITZ_LOG_WARNING( "Notification publish failed because notification-port "
                << "was not supplied on command line" );
    }
}
////////////////////////////////////////////////////////////////////////////////
void context::register_initialization_notification_handler(
    initialization_callback initialization_handler )
{
    initialization_handler_ = initialization_handler;
}
////////////////////////////////////////////////////////////////////////////////
initialization_callback context::get_initialization_notification_callback()
{
    return initialization_handler_;
}
////////////////////////////////////////////////////////////////////////////////
void context::register_inedge_message_handler(
    collaboration_callback inedge_message_handler )
{
    inedge_message_handler_ = inedge_message_handler;
}
////////////////////////////////////////////////////////////////////////////////
collaboration_callback context::get_inedge_message_handler()
{
    return inedge_message_handler_;
}
////////////////////////////////////////////////////////////////////////////////
const po::variables_map& context::get_config() const
{
    return application_configuration_;
}
////////////////////////////////////////////////////////////////////////////////
void context::send_internal_message( const char* message )
{
    std::string s( message );
    KIBITZ_LOG_INFO( "sending internal message -> " << message );
    kibitz::util::send( message_bus_socket_, s );
}
////////////////////////////////////////////////////////////////////////////////
void context::set_worker_type( const std::string& worker_type_name )
{
    worker_type_name_ = worker_type_name;
}
////////////////////////////////////////////////////////////////////////////////
void context::set_worker_id( const std::string& worker_id )
{
    worker_id_ = worker_id ;
}
////////////////////////////////////////////////////////////////////////////////
void context::start()
{
    std::string locator_binding = ( boost::format( "tcp://%1%:%2%" ) %
                                    application_configuration_[ "locator-host" ].as< std::string >() %
                                    application_configuration_[ "locator-send-port" ].as< int >() ).str();

    publisher locator_pub(
        zmq_context(),
        locator_binding,
        ZMQ_PUSH,
        INPROC_LOCATOR_PUBLISH_BINDING,
        publish::connect );

    if( application_configuration_.count( "notification-port" ) )
    {
        std::string notification_binding = ( boost::format( "tcp://*:%1%" ) %
                                        application_configuration_[ "notification-port" ].as< int >() ).str();
        KIBITZ_LOG_NOTICE( "Worker [" << worker_type_name_ << "." << worker_id_
                << "] may publish notifications on " << notification_binding );

        publisher notify_pub(
            zmq_context(),
            notification_binding,
            ZMQ_PUB,
            INPROC_NOTIFICATION_PUBLISH_BINDING,
            publish::bind );
        create_thread( notify_pub );
    }

    if( application_configuration_.count( "status-sink-binding" ) )
    {
        std::string status_sink_binding = application_configuration_["status-sink-binding"].as< std::string >();
        KIBITZ_LOG_NOTICE( "Worker [" << worker_type_name_ << "." << worker_id_
                    << "] will publish status " << status_sink_binding );
        publisher status_pub(
            zmq_context(),
            status_sink_binding,
            ZMQ_PUSH,
            INPROC_NOTIFICATION_PUBLISH_STATUS,
            publish::connect
        );
        create_thread( status_pub ) ;
        status_publisher_enabled_ = true;
    }

    heartbeat_receiver hb_receiver( this );
    kibitz::in_edge_manager in_edge_manager( *this );
    kibitz::collaboration_handler collaboration_handler( this,  INPROC_COLLABORATION_MESSAGE_HANDLER );
    create_thread( collaboration_handler ) ;
    create_thread( locator_pub );
    create_thread( in_edge_manager );
    create_thread( hb_receiver );
    send_worker_status( START );
    m_threads.join_all();
}
////////////////////////////////////////////////////////////////////////////////
void context::send_worker_status( worker_status_t status )
{
    if( !status_publisher_enabled_ )
    {
        return;
    }

    int tries = 0;
    while( tries++ < 3 )
    {
        try
        {
            std::string worker_type = application_configuration_["worker-type"].as< std::string >();
            std::string worker_id = boost::lexical_cast<std::string>( application_configuration_["worker-id"].as< int >() ) ;
            publisher p( zmq_context(), INPROC_NOTIFICATION_PUBLISH_STATUS );
            worker_status_message message( worker_type, worker_id, status );
            p.send( message.to_json() );
            return;
        }
        catch( ... )
        {
            KIBITZ_LOG_NOTICE( "Attempt to send status message failed, retrying..." );
            boost::this_thread::sleep( boost::posix_time::microseconds( 1000 ) );
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
void context::stop()
{
  m_threads.interrupt_all();
}
////////////////////////////////////////////////////////////////////////////////
void context::terminate()
{
    KIBITZ_LOG_NOTICE( "context.terminate shutting down application" );
    m_threads.interrupt_all();
    util::close_socket( message_bus_socket_ );

    if( zmq_context_ )
    {
        zmq_term( zmq_context_ );
    }
}
////////////////////////////////////////////////////////////////////////////////
void* context::zmq_context()
{
    return zmq_context_;
}
////////////////////////////////////////////////////////////////////////////////
void context::set_job_id( const std::string& job_id )
{
    boost::mutex::scoped_lock lock( mutex_ ) ;
    current_job_id_ = job_id;
}
////////////////////////////////////////////////////////////////////////////////
void context::get_job_id( std::string& job_id )
{
    boost::mutex::scoped_lock lock( mutex_ );
    job_id = current_job_id_;
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz
