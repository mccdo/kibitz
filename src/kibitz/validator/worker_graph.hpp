#ifndef __WORKER_GRAPH_HPP__
#define __WORKER_GRAPH_HPP__

#include <kibitz/common.hpp>

namespace kibitz {
  namespace graph {
    class node;
    class worker_graph;
    typedef shared_ptr< node > node_ptr_t;
    typedef std::map< string, node_ptr_t > node_map_t;
    typedef std::pair< string, node_ptr_t > node_entry_t;
    typedef std::vector< string > node_names_t;
    typedef std::pair< string, node_ptr_t> node_pair_t;
    typedef std::set< string > ordered_node_names_t;

    /// \brief Represents a worker type.
    ///
    /// Contains information about a worker, its parent workers that send it messages ( in edges) 
    /// and its children workers that recieve messages sent by this worker. 
    class node {
      node_map_t in_edges_;
      node_map_t out_edges_;

      template< class Functor  > 
      void traverse_edges( const node_map_t& nodes, Functor& f ) {
	BOOST_FOREACH(  const node_entry_t& node_entry, nodes ) {
	  f( node_entry.first, node_entry.second );
	}
      }

      node_names_t get_edges( const node_map_t& nodes ) const {
	node_names_t names;
	BOOST_FOREACH( const node_entry_t& node, nodes ) {
	  names.push_back( node.first );
	}
	return names;
      }

      void add_in_edge( const string& node_name, node_ptr_t ptr ) ;

      void add_out_edge( const string& node_name, node_ptr_t ptr ) ;

      friend class worker_graph;
    public:
      node(  ) ;
      virtual ~node() ;

      /// Passes worker name and worker node ptr of all out 
      /// edges to functor f( worker, node )
      ///
      /// See unit tests for example of usage.
      ///
      template< class Functor > 
      void traverse_out_edges( Functor& f ) {
	traverse_edges( out_edges_, f );
      }

      /// Passes worker name and worker node ptr of all out 
      /// edges to functor f( worker, node )
      ///
      /// See unit tests for example of usage.
      ///
      template< class Functor >
      void traverse_in_edges( Functor& f ) {
	traverse_edges( in_edges_, f );
      }

      /// Gets a list of workers that this worker sends messages
      /// to.
      node_names_t get_out_edges() const {
	return get_edges( out_edges_ );
      }

      /// Gets a list of workers that this worker gets messages
      /// from.
      node_names_t get_in_edges() const {
	return get_edges( in_edges_ );
      }


    };

    /// \brief Represents a collaboration graph.
    /// 
    /// Each node represents a worker and can be used to fetch information about 
    /// workers that send message to a particular worker (in edges) 
    /// or workers that a particular worker sends messages to (out edges).
    class worker_graph {
      node_map_t node_map_;
      
      worker_graph( );
      node_ptr_t get_or_add_node( const string& node_name );
      friend shared_ptr<worker_graph> create_worker_graph_from_file( const string& file_name );
      friend shared_ptr<worker_graph> create_worker_graph_from_string( const string& graph_definition );
    public:
      virtual ~worker_graph() ;
      
      /// Add a worker to the graph
      /// 
      /// \param worker_name the name of the worker.
      ///
      /// \return node pointer for the worker
      node_ptr_t add_worker( const string& worker_name );

      /// Creates an out edge
      /// \param worker_name the name of the parent worker that will send messages to child.
      /// \param child_name the name of the worker that will get messages from the parent.
      ///
      void add_worker_child( const string& worker_name, const string& child_name );
      
      /// Creates an in edge.
      /// \param worker_name the name of the worker that will get messages from parent.
      /// \param parent_name the name of the parent that will  send messages to worker_name.
      ///
      void add_worker_parent( const string& worker_name, const string& parent_name );
     
      /// Gets worker with a given name.
      /// \return node for worker. 
      node_ptr_t get_worker( const string& worker_name );

      /// Gets ordered list of names of worker types
      /// \return names of worker types in ascending order
      ordered_node_names_t get_workers() const;

      void validate( ) const ;
    };

    typedef shared_ptr< worker_graph > worker_graph_ptr;
    
    /// Creates a worker graph from text file. 
    ///
    /// \param file_name name of the file containing graph information
    /// 
    /// \return pointer to worker graph
    worker_graph_ptr create_worker_graph_from_file( const string& file_name );

    /// Creates a worker graph from text file. 
    ///
    /// \param graph_definition string containing graph information.
    /// 
    /// \return pointer to worker graph
    worker_graph_ptr create_worker_graph_from_string( const string& graph_definition );
    
    /// Strips comment delimiter and any characters on line following, returns
    /// portion of line preceding comment delimiter
    ///
    /// \param commented_line line of text from a graph configuration file which might contain comments.
    ///                 
    /// \return line with comments stripped out. 
    string strip_comments( const string& commented_line ) ;
  }
}
 


#endif
