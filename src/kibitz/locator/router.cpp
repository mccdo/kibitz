
#include <kibitz/locator/router.hpp>

#include <boost/foreach.hpp>
#include <boost/thread.hpp>


namespace ku = kibitz::util;

namespace kibitz
{
namespace locator
{

////////////////////////////////////////////////////////////////////////////////
router::router(
    void* context,
    const publisher& pub,
    const std::string& listener_binding,
    const binding_map_t& bindings,
    worker_graph_ptr graph_ptr )
    :
    context_( context ),
    publisher_( pub ),
    listener_binding_( listener_binding ),
    push_bindings_( bindings ),
    graph_ptr_( graph_ptr ),
    m_logger( Poco::Logger::get("router") )
{
    m_logStream = LogStreamPtr( new Poco::LogStream( m_logger ) );
    KIBITZ_LOG_NOTICE( "Instantiating router" );
}
////////////////////////////////////////////////////////////////////////////////
router::~router()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
void router::operator()()
{
    KIBITZ_LOG_NOTICE( "Starting router thread" );

    send_sockets_t out_socks;

    try
    {
	boost::this_thread::sleep( boost::posix_time::seconds( 1 ) );
	messages_by_worker_and_job_t inedge_cache;

	ku::sockman_ptr_t message_listener =
	    ku::create_socket_ptr( context_, ZMQ_PULL );
	ku::check_zmq( zmq_bind(
			   *message_listener, listener_binding_.c_str() ) );

	util::sockman notification_socket( publisher_.get_publish_socket() );

	bind_out_sockets( out_socks );

	while( true )
	{
	    KIBITZ_LOG_DEBUG( "Router waiting for  message" );
	    std::string json;
	    if( ku::recv_async( *message_listener, json ) ) {
		KIBITZ_LOG_DEBUG( "Router got message " << json );
		message_ptr_t message_ptr = message_factory( json );

		if( message_ptr->message_type() == NOTIFICATION_MESSAGE_TYPE )
		{
		    KIBITZ_LOG_DEBUG( "Router received notification message" );
		    publisher_.send( notification_socket, json );
		}
		else if( message_ptr->message_type() == COLLABORATION_MESSAGE_TYPE )
		{
		    KIBITZ_LOG_DEBUG( "Router recieved collaboration message " << json );
		    route_message( out_socks, json, inedge_cache );
		}
		else
		{
		    KIBITZ_LOG_WARNING( "Unknown message type received." << std::endl
					<< "Unknown Message = " << json );
		}
	    } 
	    else 
	    {
		const int one_tenth_second = 100;
		ku::wait( one_tenth_second );
	    }

	    boost::this_thread::interruption_point();
	}
    }
    catch( boost::thread_interrupted const & ) 
    {
	KIBITZ_LOG_INFO( "Router thread was interrupted" );
    }
    catch( const std::exception& ex )
    {
	KIBITZ_LOG_ERROR( "Router thread was killed. Reason " << ex.what() );
    }
}
////////////////////////////////////////////////////////////////////////////////
void router::bind_out_sockets( send_sockets_t& send_sockets )
{
    BOOST_FOREACH( const binding_pair_t & binding_pair, push_bindings_ )
    {
        void* sock = ku::create_socket( context_, ZMQ_PUSH );
        KIBITZ_LOG_DEBUG( "creating socket for [" << binding_pair.first << "] " << sock );
        int port = get_port( binding_pair.second );
        std::string push_binding = ( boost::format( "tcp://*:%1%" ) % port ).str();
        KIBITZ_LOG_INFO( "BINDING " << sock << " to " << push_binding );
        ku::sockman_ptr_t sp( new ku::sockman( sock ) );
        int rc = zmq_bind( *sp, push_binding.c_str() );
        if( rc )
        {
            KIBITZ_LOG_ERROR( "zmq bind failed - " << zmq_strerror( zmq_errno() ) );
        }
        send_sockets[ binding_pair.first ] = sp;
    }
}
////////////////////////////////////////////////////////////////////////////////
void router::route_message(
    const send_sockets_t& send_sockets,
    const std::string& json,
    messages_by_worker_and_job_t& inedge_cache )
{
    basic_collaboration_message_ptr_t msg =
        dynamic_pointer_cast< basic_collaboration_message >(
            message_factory( json ) );
    std::string sending_worker_type = msg->worker_type();
    node_ptr_t sending_worker_node =
        graph_ptr_->get_worker( sending_worker_type );
    if( sending_worker_node != NULL )
    {
        BOOST_FOREACH( const std::string & target_worker_type,
                       sending_worker_node->get_out_edges() )
        {
            KIBITZ_LOG_DEBUG( "Preparing to route to " << target_worker_type );
            send_sockets_t::const_iterator it =
                send_sockets.find( target_worker_type );
            if( it != send_sockets.end() )
            {
                KIBITZ_LOG_INFO( "Routed collaboration message for "
                        << target_worker_type );
                collaboration_message_bundle_ptr_t messages =
                    populate_inedge_messages(
                        target_worker_type,
                        graph_ptr_,
                        msg,
                        inedge_cache );
                if( inedges_have_messages( messages ) )
                {
                    KIBITZ_LOG_DEBUG( "Routing message for ["
                            << target_worker_type << "] " );

                    std::string msg = messages->to_json();
                    int rc = zmq_send(
                                 *( it->second ), msg.data(), msg.length(), ZMQ_DONTWAIT );
                    if( rc < 0 )
                    {
                        KIBITZ_LOG_WARNING( "Dropped message bound for ["
                                << target_worker_type << "]. Reason - "
                                << zmq_strerror( zmq_errno() ) );
                    }
                }
            }
            else
            {
                KIBITZ_LOG_WARNING( "Got a collaboration message that we dropped "
                        << "because its target is not in graph definition. Type = "
                        << target_worker_type );
            }
        }
    }
    else
    {
        KIBITZ_LOG_WARNING( "Sending worker " << sending_worker_type
                << " is not in graph definition. Dropping message" );
    }
}
////////////////////////////////////////////////////////////////////////////////
bool router::inedges_have_messages(
    collaboration_message_bundle_ptr_t messages ) const
{
    return ( messages != NULL );
}
////////////////////////////////////////////////////////////////////////////////
collaboration_message_bundle_ptr_t router::populate_inedge_messages(
    const target_worker_name_t& target_worker,
    worker_graph_ptr graph,
    basic_collaboration_message_ptr_t new_message,
    messages_by_worker_and_job_t& cache ) const
{
    collaboration_message_bundle_ptr_t messages;

    if( cache.count( target_worker ) == 0 )
    {
        cache[ target_worker ] = jobids_messages_t();
    }

    if( cache[ target_worker ].count( new_message->job_id() ) == 0 )
    {
        cache[ target_worker ][ new_message->job_id() ] = inedge_messages_t();
    }

    cache[ target_worker ][ new_message->job_id() ][
        new_message->worker_type() ] = new_message;

    node_ptr_t target_worker_node = graph->get_worker( target_worker ) ;
    inedge_messages_t inedge_messages =
        cache[ target_worker ][ new_message->job_id() ];
    // if we have messages on each in edge we bundle the messages up and
    // return them so they can be sent to target worker
    if( target_worker_node->get_in_edges().size() == inedge_messages.size() )
    {
        collaboration_message_ptrs_t collaboration_messages;

        BOOST_FOREACH( inedge_message_pair_t message, inedge_messages )
        {
            collaboration_messages.push_back( message.second );
        }

        messages = collaboration_message_bundle_ptr_t(
                       new collaboration_message_bundle( collaboration_messages ) );
        // ditch the record associated with this job
        cache[ target_worker ].erase( new_message->job_id() );
    }

    return messages;
}
////////////////////////////////////////////////////////////////////////////////

} //end locator
} //end kibitz
