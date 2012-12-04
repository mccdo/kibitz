#include <kibitz/locator/binding_broadcaster.hpp>

namespace kibitz {

  binding_broadcaster::binding_broadcaster( const publisher& pub, 
					    kg::worker_graph_ptr worker_graph_ptr )
    :publisher_( pub ),
     worker_graph_( worker_graph_ptr ) {
  }

  binding_broadcaster::~binding_broadcaster() {
  }

  void binding_broadcaster::operator()() {
  }
}
