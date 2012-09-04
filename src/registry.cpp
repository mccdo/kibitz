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
#include "registry.hpp"
#include <zmq.h>
#include <sys/time.h>
#include "locator.hpp"
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "worker_broadcast_message.hpp"
#include "job_initialization_message.hpp"

using boost::property_tree::ptree;

using namespace google;
using boost::dynamic_pointer_cast;

registry::registry( void* inproc_pub_socket, void* inproc_sub_socket,  void* publisher_socket ) 
  :publisher_socket_( publisher_socket ),
   inproc_pub_socket_(inproc_pub_socket),
   inproc_sub_socket_(inproc_sub_socket) {
   }

registry::~registry() {

}


void registry::push_message( const string& msg ) {
  DLOG(INFO) << "inproc message send " << msg;
  kibitz::util::send( inproc_pub_socket_, msg );
}


void registry::push_message( const kibitz::message& message ) {
  DLOG(INFO) << "sending message";
  kibitz::util::send( inproc_pub_socket_, message.to_json() );
}



void registry::operator()() {
  
  DLOG(INFO) << "starting registry thread";
  worker_type_map_t workers;
  timeval last_send = { 0, 0 };

  try {
    while( true ) {
      string message;
      kibitz::util::recv( inproc_sub_socket_, message ) ;
      DLOG(INFO) << "Registry got a message " << message ;
      kibitz::message_ptr_t message_ptr = kibitz::message_factory( message );
      assert( message_ptr->message_type() == "notification" );
      string notification_type = dynamic_pointer_cast<kibitz::notification_message>(message_ptr)->notification_type();
      
      if( notification_type == "inproc" ) {
	int notification = dynamic_pointer_cast<kibitz::inproc_notification_message>(message_ptr)->get_notification() ;
	bool exit_thread = false;
	switch( notification ) {
	case kibitz::message::stop :
	  exit_thread = true;
	  break;
	default :
	  DLOG(INFO) << "Got a message we did not handle. notification = " << notification ;
	  break;
	}

	if( exit_thread ) {
	  break;
	}
      }

      if( notification_type == "worker_broadcast" ) {
	
	kibitz::worker_broadcast_message_ptr_t broadcast_ptr = dynamic_pointer_cast<kibitz::worker_broadcast_message>( message_ptr );
	LOG(INFO) << "Publishing " << broadcast_ptr->notification() << " to all workers";
	kibitz::util::send( publisher_socket_, broadcast_ptr->to_json() );
      }

      if( notification_type == kibitz::notification::JOB_INITIALIZATION ) {
	LOG(INFO) << "Publishing notification type -> " << notification_type ;
	kibitz::util::send( publisher_socket_, message_ptr->to_json() );
      }

      if( notification_type == "heartbeat" ) {
	heartbeat_ptr_t heartbeat_ptr = dynamic_pointer_cast<kibitz::heartbeat>(message_ptr);
	workers[heartbeat_ptr->worker_type()][heartbeat_ptr->worker_id()] = heartbeat_ptr;

	
	if( one_second_elapsed( last_send ) ) {
	  DLOG(INFO) << "Sending locator information for " << workers.size() << " workers";
	  for( worker_type_map_t::iterator worker_type = workers.begin(); worker_type != workers.end(); ++worker_type ) {
	    DLOG(INFO) << "Sending Message for worker type " << worker_type->first ;
	    for( worker_map_t::iterator worker = worker_type->second.begin(); worker != worker_type->second.end(); ++worker ) {
	      DLOG(INFO) << "Sending worker type " << worker_type->first << " worker id " << worker->first ;
	      kibitz::util::send( publisher_socket_, worker->second->to_json() );	      
	    }
	  }

	}
	
	
      }
    }
  } catch( const kibitz::util::queue_interrupt& ) {
    LOG(INFO) << "Shutting down sender thread";
  } catch( const std::exception& ex ) {
    LOG(ERROR) << "An exception killed sender thread " << __FILE__ << " " << __LINE__ ;
  }

  
}

// TODO this method controls frequency of broadcast of 
// locator information, we want to make it configurable
bool registry::one_second_elapsed( timeval& last_send ) {
  bool second_elapsed = false;
  const int microsecs_in_second = 1000000;
  timeval current_time;
  gettimeofday( &current_time, NULL );
  long curr_usec = (current_time.tv_sec * microsecs_in_second) + current_time.tv_usec;
  long last_usec = (last_send.tv_sec * microsecs_in_second) +  last_send.tv_usec;
  if( (curr_usec - last_usec) > microsecs_in_second ) {
    last_send = current_time;
    second_elapsed = true;
  }
  return second_elapsed;
  
}
