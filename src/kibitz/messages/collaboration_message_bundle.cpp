#include <kibitz/messages/collaboration_message_bundle.hpp>


namespace kibitz {
  const char* collaboration_message_bundle::MESSAGE_TYPE = "collaboration_messages";


  collaboration_message_bundle::collaboration_message_bundle( const collaboration_message_ptrs_t& messages )
    :collaboration_message( collaboration_message_bundle::MESSAGE_TYPE ) {
  }

  collaboration_message_bundle::collaboration_message_bundle( const ptree& tree )
    :collaboration_message( collaboration_message_bundle::MESSAGE_TYPE ) { 
  }

  collaboration_message_bundle::~collaboration_message_bundle() {
  }


  string collaboration_message_bundle::to_json() const {
  }

}
