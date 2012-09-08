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
#include <kibitz/worker_map.hpp>
#include <kibitz/messages/inproc_notification_message.hpp>

namespace kibitz {


  worker_map::worker_map(context* context) 
    :send_socket_(NULL),
     context_(context)  {
    //send_socket_ = util::create_socket( context_->zmq_context(), ZMQ_REQ );
    //util::check_zmq( zmq_connect( send_socket_, WORKER_MANAGER_BINDING )); 
  }

  worker_map::~worker_map() {
    util::close_socket( send_socket_ );
  }

  worker_map::worker_map( void* send_socket ) 
    :send_socket_(send_socket),
     context_(NULL) {
  }

  shared_ptr<worker_map> worker_map::get_worker_map( void* context ) {
    void* socket = NULL;
    socket = util::create_socket( context, ZMQ_REQ );
    util::check_zmq( zmq_connect( socket, WORKER_MANAGER_BINDING ));
    return shared_ptr<worker_map>( new worker_map( socket ) );
  }

  worker_infos_t worker_map::get_in_edge_workers(const string& worker_type ) const {
    worker_query query( worker_type );
    util::send( send_socket_, query.to_json() );
    string response_json;
    util::recv( send_socket_, response_json );
    worker_query_response_ptr_t worker_query_response_ptr = dynamic_pointer_cast<worker_query_response>( message_factory( response_json ) ) ;
    return worker_query_response_ptr->get_workers() ;
  }

  void worker_map::send_worker_notification_from_heartbeat( const string& json ) {
    DLOG(INFO) << "WORKER NOTIFY " << json;
    heartbeat_ptr_t heartbeat_ptr = dynamic_pointer_cast<heartbeat>( message_factory( json ) ) ;
    worker_notification_message message( heartbeat_ptr );
    util::send( send_socket_, message.to_json() );
    string response;
    util::recv( send_socket_,response );
    inproc_notification_ptr_t notification_ptr = dynamic_pointer_cast<inproc_notification_message>(message_factory( response ));
   
    if( notification_ptr->get_notification() != message::ok ) {
      throw std::runtime_error( "Attempt to send worker notification failed" );
    }
       
  }

  void worker_map::handle_worker_notification( message_ptr_t message, worker_map_t& workers )  {
    DLOG(INFO) << "handling worker notification" ;
    worker_notification_message_ptr_t worker = dynamic_pointer_cast<worker_notification_message>(message);
    // TODO timestamp when worker was placed into this map 
    // so we can deal with dead workers that don't heartbeat
    workers[worker->worker_type()][worker->worker_id()] = worker;
  }

  worker_infos_t worker_map::process_query( const string& worker_type, const worker_map_t& workers ) const {
    worker_infos_t response;
    worker_map_t::const_iterator hits = workers.find( worker_type ) ;
    if( hits != workers.end() ) {
      typedef const std::pair< worker_id_t, worker_notification_message_ptr_t >&  pair_t;
      BOOST_FOREACH( pair_t hit, hits->second ) {
	worker_info worker ;
	worker.worker_type = hit.second->worker_type();
	worker.worker_id = hit.second->worker_id();
	worker.port = hit.second->port();
	worker.host = hit.second->host_name() ;
	response.push_back( worker );	
      }
    }
    return response;
    
  }

void worker_map::operator()() 
{
    DLOG(INFO) << "Worker map manager thread started";
    worker_map_t worker_map;
    void* socket = NULL;
    try 
    {
        socket = util::create_socket( context_->zmq_context(), ZMQ_REP );
        util::check_zmq( zmq_bind( socket, WORKER_MANAGER_BINDING ) );
        while( true ) 
        {
            string json ;
            util::recv( socket, json );
            DLOG(INFO) << context_->worker_type() << ":" << context_->worker_id() << " worker map manager got " << json;
            message_ptr_t message = message_factory( json );
            CHECK( message->message_type() == "notification" ) << "didn't get expected notification message";
            string notification_type = dynamic_pointer_cast<notification_message>(message)->notification_type(); 
            if( notification_type == "worker_notification" )
            {
                handle_worker_notification( message, worker_map );
                inproc_notification_message response( message::ok );
                util::send( socket, response.to_json() );
            }

            if( notification_type == "worker_query" ) 
            {
                worker_query_ptr_t query_ptr = dynamic_pointer_cast<worker_query>( message );
                //worker_infos_t worker_infos;
                DLOG(INFO) << "WORKER QUERY BY " << context_->worker_type() << ":" << context_->worker_id() ;
                worker_map_t::iterator hits = worker_map.find( query_ptr->worker_type() ) ;
                worker_infos_t worker_infos = process_query( query_ptr->worker_type(), worker_map );
                worker_query_response wqr;
                wqr.set_workers( worker_infos );
                string wqr_response = wqr.to_json();
                DLOG(INFO) << "Sending " << context_->worker_type() << ":" << context_->worker_id() << " query response " << wqr_response;
                util::send( socket, wqr_response );
            }
        }
    } 
    catch( const kibitz::util::queue_interrupt& ) 
    {
        LOG(INFO) << "Worker map manager got interrupt shutting down";
    } 
    catch( const std::exception& ex ) 
    {
        LOG(ERROR) << "Exception: " << ex.what() ;
    }

    util::close_socket( socket );
}
}
