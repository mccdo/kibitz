#ifndef kibitz_locator_common_hpp
#define kibitz_locator_common_hpp

#include <string>
#include <map>
#include <kibitz/validator/validator.hpp>

using kibitz::graph::worker_graph_ptr;

namespace kibitz {
  namespace locator {
    typedef std::string worker_type_name_t;
    typedef std::string worker_binding_t;						
    typedef std::map<worker_type_name_t, worker_binding_t > binding_map_t;

    /// Creates bindings workers will use to as source of collaboration messages.
    ///
    /// \param binding_root binding without the port. For example tcp://192.168.1.2
    /// \param graph object populated with collaboration graph.
    /// \param base_port starting port for binding assignments.
    /// \param [in,out] will be returned with list of a binding for each worker. A binding
    /// with the port is returned, each worker type will have a different port. 
    void create_bindings( const worker_binding_t& binding_root, 
			  worker_graph_ptr graph, 
			  int base_port,
			  binding_map_t& bindings ); 
 }
}

#endif
