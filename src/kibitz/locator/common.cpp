#include <kibitz/locator/common.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

using boost::format;

namespace kibitz {
  namespace locator {
    void create_bindings( const worker_binding_t& binding_root, 
		       worker_graph_ptr graph, 
		       int base_port, 
		       binding_map_t& bindings ) {
      
      BOOST_FOREACH( const worker_type_name_t& worker_type, graph->get_workers() ) {
	bindings[worker_type] = (format( "%1%:%2%" ) % binding_root % base_port++ ).str();
      }
    }
  }
}

