#include <kibitz/collaboration_handler.hpp>
#include <kibitz/messages/basic_collaboration_message.hpp>
#include <boost/foreach.hpp>

namespace kibitz {


  collaboration_handler::collaboration_handler( context* ctx, collaboration_message_bundle_ptr_t message ) 
    :context_( ctx ),
     collaboration_bundle_( message ) {
  }


  collaboration_handler::~collaboration_handler() {
  }

  void collaboration_handler::operator() () {
    VLOG(1) << "Entered collaboration handler thread";
    collaboration_callback cb = context_->get_inedge_message_handler();
    if( cb ) {
      VLOG( 1 ) << "Invoking collaboration callback" ;
      try {
	collaboration_messages_t payloads;
	string job_id ;

	BOOST_FOREACH( collaboration_message_ptr_t message, collaboration_bundle_->messages() ) {
	  VLOG(1) << "GOT COLLAB MESSAGES";
	  basic_collaboration_message_ptr_t payload_message = static_pointer_cast<basic_collaboration_message>(message);
	  job_id = payload_message->job_id();	  
	  payloads.push_back( payload_message->payload() );
	}
	LOG(INFO) << "Invoking message handler for worker type [" 
		  << context_->worker_type() 
		  << "], Job [" << job_id << "]";
	context_->set_job_id( job_id ) ;
	cb( payloads );
      } catch( ... ) {
	LOG(ERROR) << "Collaboration callback threw an exception";
      }
    } else {
      LOG(ERROR) << "Recieved a message from another worker that "
	"there is not a callback defined to handle.";
    }
  }

}
