#ifndef kibitz_locator_common_hpp
#define kibitz_locator_common_hpp

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <kibitz/validator/validator.hpp>
#include <kibitz/kibitz_util.hpp>
#include <glog/logging.h>


using kibitz::graph::worker_graph_ptr;
using std::string;
using boost::shared_ptr;
using boost::format;

namespace kibitz {
  namespace locator {
    typedef std::string worker_type_name_t;
    typedef std::string worker_binding_t;						
    typedef std::map<worker_type_name_t, worker_binding_t > binding_map_t;
    typedef std::pair< worker_type_name_t, worker_binding_t > binding_pair_t;

    /// \brief Creates bindings workers will use to as source of collaboration messages.
    ///
    /// \param binding_root binding without the port. For example tcp://192.168.1.2
    /// \param graph object populated with collaboration graph.
    /// \param base_port starting port for binding assignments.
    /// \param bindings [in,out] will be returned with list of a binding for each worker. A binding
    /// with the port is returned, each worker type will have a different port. 
    void create_bindings( const worker_binding_t& binding_root, 
			  worker_graph_ptr graph, 
			  int base_port,
			  binding_map_t& bindings );

    /// Gets port from string binding
    /// \param binding string of the form tcp://foo.com:123345
    /// \return port 
    int get_port( const string& binding ) ;
 }
}

#endif
