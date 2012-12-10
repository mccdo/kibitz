#include <kibitz/messages/collaboration_message_bundle.hpp>
#include <boost/foreach.hpp>

namespace kibitz {
  const char* collaboration_message_bundle::MESSAGE_TYPE = "collaboration_messages";


  collaboration_message_bundle::collaboration_message_bundle( const collaboration_message_ptrs_t& messages )
    :collaboration_message( collaboration_message_bundle::MESSAGE_TYPE ),
     messages_(messages) {
  }

  collaboration_message_bundle::collaboration_message_bundle( const ptree& tree )
    :collaboration_message( collaboration_message_bundle::MESSAGE_TYPE ) { 
   
    BOOST_FOREACH( const ptree::value_type& message_tree, tree.get_child( "messages" ) ) {
      basic_collaboration_message_ptr_t message_ptr = basic_collaboration_message::from_ptree( message_tree.second );
      messages_.push_back( message_ptr );
    }
  }

  collaboration_message_bundle::~collaboration_message_bundle() {
  }


  string collaboration_message_bundle::to_json() const {
    stringstream stm;
    ptree tree;
    collaboration_message::populate_header( tree );
    ptree array ;

    BOOST_FOREACH( basic_collaboration_message_ptr_t message, messages_ ) {
      ptree message_tree;
      message->to_ptree( message_tree );
      array.push_back( std::make_pair( "", message_tree ) );      
    }

    tree.push_back( std::make_pair( "messages", array ) );
    boost::property_tree::json_parser::write_json( stm, tree );
    return stm.str();
    
  }

}
