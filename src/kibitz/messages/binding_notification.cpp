#include <kibitz/messages/binding_notification.hpp>

namespace kibitz {
  const char* binding_notification::NOTIFICATION_TYPE = "binding";


  binding_notification::binding_notification( const string& target_worker, const string& zmq_binding ) 
    :notification_message( binding_notification::NOTIFICATION_TYPE ),
     target_worker_( target_worker ),
     zmq_binding_( zmq_binding ) {
  }

  binding_notification::binding_notification( const ptree& json ) 
    :notification_message( binding_notification::NOTIFICATION_TYPE ),
     target_worker_( json.get<string>("target_worker")),
     zmq_binding_( json.get<string>("zmq_binding") ) {
  }

  binding_notification::~binding_notification( ) {
  }


  string binding_notification::to_json() const {
    stringstream stm;
    ptree tree;
    notification_message::populate_header( tree );
    tree.put( "target_worker", target_worker_ );    
    tree.put( "zmq_binding", zmq_binding_ );
    boost::property_tree::json_parser::write_json( stm, tree );
    return stm.str();
  }
  
}
