#include <kibitz/initialization_handler.hpp>

namespace kibitz {


  initialization_handler::initialization_handler( context* ctx, const string& initialization_data ) 
    :context_( ctx ),
     initialization_data_( initialization_data ) {
  }


  initialization_handler::~initialization_handler() {
  }

  void initialization_handler::operator() () {
    VLOG(1) << "Entered initialization handler thread";
    initialization_callback cb = context_->get_initialization_notification_callback();
    if( cb ) {
      VLOG( 1 ) << "Invoking initialization callback" ;
      try {
	cb( initialization_data_ );
      } catch( ... ) {
	LOG(ERROR) << "Job initialization callback threw an exception";
      }
    }
  }

}
