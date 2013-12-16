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
#include <kibitz/collaboration_handler.hpp>
#include <kibitz/messages/basic_collaboration_message.hpp>
#include <boost/foreach.hpp>



namespace kibitz
{



////////////////////////////////////////////////////////////////////////////////
collaboration_handler::collaboration_handler(
					     context* ctx,
					     const string& binding ) 
    :
  context_( ctx ),
  binding_( binding )
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
collaboration_handler::~collaboration_handler()
{
    ;
}


////////////////////////////////////////////////////////////////////////////////
  void collaboration_handler::operator ()()
  {
    VLOG( 1 ) << "Entered collaboration handler thread";

    try {
      collaboration_callback cb = context_->get_inedge_message_handler();

      if( cb ) {

	util::sockman sock( context_->zmq_context(), ZMQ_PAIR );
	util::check_zmq( zmq_connect( sock, binding_.c_str() ) );
      
	while( true ) {
	  string json;
	  util::recv( sock, json );

	  context_->decrement_collaboration_queue() ;
	  collaboration_message_bundle_ptr_t bundle = static_pointer_cast< collaboration_message_bundle >( message_factory( json ) );
	  collaboration_messages_t payloads;
	  string job_id;

	  VLOG(1) << "Got collaboration messages";
	
	  BOOST_FOREACH( collaboration_message_ptr_t msg, bundle->messages() ) {
	    basic_collaboration_message_ptr_t payload_message = static_pointer_cast< basic_collaboration_message >( msg ) ;
	    job_id = payload_message->job_id();
	    payloads.push_back( payload_message->payload() );
	  }
        
	  VLOG( 1 )
	    << "Invoking message handler for worker type ["
	    << context_->worker_type() << "], Job [" << job_id << "]";
	  // if end user screws up and doesn't catch something in 
	  // their code, we need to handle it so it doesn't nuke our thread
	  try {
	    context_->set_job_id( job_id );
	    cb( payloads );
	  } catch( ... ) {
	    LOG(ERROR) << "Caught an unhandled exception in collaboration callback. " << __FILE__ << ":" << __LINE__;
	  }       
	
	}
      }

    } catch( std::exception const& e ) {
      LOG( ERROR ) << "An exception occurred in collaboration handler thread. " << e.what() <<  " Worker  " << context_->worker_type() 
		   << ":" << context_->worker_id() ; 

    }


  }
  ////////////////////////////////////////////////////////////////////////////////

} //end kibitz
