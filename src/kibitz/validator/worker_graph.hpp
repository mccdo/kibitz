#ifndef __WORKER_GRAPH_HPP__
#define __WORKER_GRAPH_HPP__

#include <kibitz/common.hpp>

namespace kibitz {
  namespace graph {
    class node;
    class worker_graph;
    typedef shared_ptr< node > node_ptr;
    typedef std::map< string, node_ptr > node_map_t;
    //    typedef std::vector< string > node_names_t;

    class node {
      node_map_t in_edges_;
      node_map_t out_edges_;

      template< class Functor  > 
      void traverse_edges( const node_map_t& nodes, Functor& f ) {
      }

      void add_in_edge( const string& node_name, node_ptr ptr ) ;

      void add_out_edge( const string& node_name, node_ptr ptr ) ;

      friend class worker_graph;
    public:
      node(  ) ;
      virtual ~node() ;

      template< class Functor > 
      void traverse_out_edges( Functor& f ) {
	traverse_edges( out_edges_, f );
      }

      template< class Functor >
      void traverse_in_edges( Functor& f ) {
	traverse_edges( in_edges_, f );
      }

    };
    
    class worker_graph {
      node_map_t node_map_;
      
      worker_graph( );
      node_ptr get_or_add_node( const string& node_name );
      friend shared_ptr<worker_graph> create_worker_graph_from_file( const string& file_name );
      friend shared_ptr<worker_graph> create_worker_graph_from_string( const string& graph_definition );
    public:
      virtual ~worker_graph() ;
      node_ptr add_worker( const string& worker_name );
      void add_worker_child( const string& worker_name, const string& child_name );
      void add_worker_parent( const string& worker_name, const string& parent_name );
      node_ptr get_worker( const string& worker_name );


      void validate( ) const ;
    };

    typedef shared_ptr< worker_graph > worker_graph_ptr;

    worker_graph_ptr create_worker_graph_from_file( const string& file_name );
    worker_graph_ptr create_worker_graph_from_string( const string& graph_definition );
    /**
     * Strips comment delimiter and any characters on line following, returns
     * portion of line preceding comment delimiter
     */
    string strip_comments( const string& commented_line ) ;
  }
}
 


#endif
