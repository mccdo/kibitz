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
					     const std::string& binding ) 
    :
  context_( ctx ),
binding_( binding ),
    m_logger( Poco::Logger::get("collaboration_handler") )
{
    m_logStream = LogStreamPtr( new Poco::LogStream( m_logger ) );
}
////////////////////////////////////////////////////////////////////////////////
collaboration_handler::~collaboration_handler()
{
    ;
}


////////////////////////////////////////////////////////////////////////////////

  void collaboration_handler::operator ()()
  {
    KIBITZ_LOG_DEBUG( "Entered collaboration handler thread" );

    try {
      collaboration_callback cb = context_->get_inedge_message_handler();

      if( cb ) {

	util::sockman sock( context_->zmq_context(), ZMQ_PAIR );
	util::check_zmq( zmq_connect( sock, binding_.c_str() ) );
      
	while( true ) {
	  std::string json;
	  util::recv( sock, json );

	  context_->decrement_collaboration_queue() ;
	  collaboration_message_bundle_ptr_t bundle = static_pointer_cast< collaboration_message_bundle >( message_factory( json ) );
	  collaboration_messages_t payloads;
	  std::string job_id;

	  KIBITZ_LOG_DEBUG( "Got collaboration messages" );
	
	  BOOST_FOREACH( collaboration_message_ptr_t msg, bundle->messages() ) {
	    basic_collaboration_message_ptr_t payload_message = static_pointer_cast< basic_collaboration_message >( msg ) ;
	    job_id = payload_message->job_id();
	    payloads.push_back( payload_message->payload() );
	  }
        
	  KIBITZ_LOG_DEBUG((boost::format("Job %1% received by  worker %2%:%3%.") % job_id % context_->worker_type() % context_->worker_id() ).str() );  

	  // if end user screws up and doesn't catch something in 
	  // their code, we need to handle it so it doesn't nuke our thread
	  try {
	    context_->set_job_id( job_id );
	    cb( payloads );
	  } catch( ... ) {
	    KIBITZ_LOG_ERROR("Caught an unhandled exception in collaboration callback.");
	  }       
	
	}
      }

    } catch( std::exception const& e ) {
      KIBITZ_LOG_ERROR((boost::format("An exception occurred in collaboration handler thread. %1% Worker %2%:%3%.") % e.what() % 
			context_->worker_type() % context_->worker_id() ).str() ) ; 
    }


  }
  ////////////////////////////////////////////////////////////////////////////////

} //end kibitz
