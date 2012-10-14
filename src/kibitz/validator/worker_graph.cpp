#include "worker_graph.hpp"


namespace kibitz {
  namespace graph {
    node::node( ) { 

    }

    node::~node() {
    }

    void node::add_in_edge( const string& node_name, node_ptr ptr ) {
      in_edges_[ node_name ] = ptr;
    }

    void node::add_out_edge( const string& node_name, node_ptr ptr ) {
      out_edges_[ node_name ] = ptr;
    }

    worker_graph::worker_graph() {

    }

    worker_graph::~worker_graph() {
    }

    node_ptr worker_graph::get_or_add_node( const string& node_name ) {

      while( !node_map_.count( node_name ) ) {
	node_map_[node_name] = node_ptr( new node() );
      }

      return node_map_[node_name];

    }

    node_ptr worker_graph::add_worker( const string& worker_name ) {
      return get_or_add_node( worker_name );
    }

    void worker_graph::add_worker_child( const string& worker_name, const string& child_name ) {
      node_ptr begin_node = get_or_add_node( worker_name );
      node_ptr end_node = get_or_add_node( child_name );
      begin_node->add_out_edge( child_name, end_node );
    }

    void worker_graph::add_worker_parent( const string& worker_name, const string& parent_name ) {
      node_ptr end_node = get_or_add_node( worker_name );
      node_ptr begin_node = get_or_add_node( parent_name );
      end_node->add_in_edge( parent_name, begin_node );
    }

    node_ptr worker_graph::get_worker( const string& worker_name ) {
      node_ptr np;
      if( node_map_.count( worker_name ) ) { 
	np = node_map_[ worker_name ];
      }

      return np;
    }

    void worker_graph::validate( ) const {
    }

    worker_graph_ptr create_worker_graph_from_file( const string& file_name ) {
      worker_graph_ptr ptr ;
      return ptr;
    }

    worker_graph_ptr create_worker_graph_from_string( const string& graph_definition ) {
      worker_graph_ptr ptr;
      return ptr;
    }



  }
}
