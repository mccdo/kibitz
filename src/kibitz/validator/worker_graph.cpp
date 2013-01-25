#include "worker_graph.hpp"
#include <boost/tokenizer.hpp>
#include <fstream>
#include <sstream>
#include <boost/foreach.hpp>

typedef boost::char_separator< char > separator_t;
typedef boost::tokenizer< separator_t > tokenizer_t;


namespace kibitz
{
namespace graph
{

////////////////////////////////////////////////////////////////////////////////
node::node()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
node::~node()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
void node::add_in_edge( const string& node_name, node_ptr_t ptr )
{
    in_edges_[ node_name ] = ptr;
}
////////////////////////////////////////////////////////////////////////////////
void node::add_out_edge( const string& node_name, node_ptr_t ptr )
{
    out_edges_[ node_name ] = ptr;
}
////////////////////////////////////////////////////////////////////////////////
worker_graph::worker_graph()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
worker_graph::~worker_graph()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
node_ptr_t worker_graph::get_or_add_node( const string& node_name )
{
    while( !node_map_.count( node_name ) )
    {
        node_map_[ node_name ] = node_ptr_t( new node() );
    }

    return node_map_[ node_name ];
}
////////////////////////////////////////////////////////////////////////////////
node_ptr_t worker_graph::add_worker( const string& worker_name )
{
    return get_or_add_node( worker_name );
}
////////////////////////////////////////////////////////////////////////////////
void worker_graph::add_worker_child(
    const string& worker_name,
    const string& child_name )
{
    node_ptr_t begin_node = get_or_add_node( worker_name );
    node_ptr_t end_node = get_or_add_node( child_name );
    begin_node->add_out_edge( child_name, end_node );
}
////////////////////////////////////////////////////////////////////////////////
void worker_graph::add_worker_parent(
    const string& worker_name,
    const string& parent_name )
{
    node_ptr_t end_node = get_or_add_node( worker_name );
    node_ptr_t begin_node = get_or_add_node( parent_name );
    end_node->add_in_edge( parent_name, begin_node );
}
////////////////////////////////////////////////////////////////////////////////
node_ptr_t worker_graph::get_worker( const string& worker_name )
{
    node_ptr_t np;
    if( node_map_.count( worker_name ) )
    {
        np = node_map_[ worker_name ];
    }

    return np;
}
////////////////////////////////////////////////////////////////////////////////
ordered_node_names_t worker_graph::get_workers() const
{
    ordered_node_names_t result;
    BOOST_FOREACH( const node_pair_t& node, node_map_ )
    {
        result.insert( node.first );
    }

    return result;
}
////////////////////////////////////////////////////////////////////////////////
void worker_graph::validate() const
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
worker_graph_ptr create_worker_graph_from_file( const string& file_name )
{
    std::ifstream stm( file_name.c_str() );
    if( !stm.good() )
    {
        throw std::runtime_error(
            ( format( "Unable to read graph file %1%" ) % file_name ).str() );
    }

    std::stringbuf buf;

    stm.get( buf, 0 );

    return create_worker_graph_from_string( buf.str() );
}
////////////////////////////////////////////////////////////////////////////////
string strip_comments( const string& commented_line )
{
    const char COMMENT_DELIMITER = '#';
    std::string result ;
    BOOST_FOREACH( char c, commented_line )
    {
        if( c == COMMENT_DELIMITER ) break;
        result += c;
    }

    return result ;
}
////////////////////////////////////////////////////////////////////////////////
worker_graph_ptr create_worker_graph_from_string(
    const string& graph_definition )
{
    worker_graph_ptr graph_ptr( new worker_graph() ) ;
    separator_t line_delimiter( "\n" );
    tokenizer_t line_tokenizer( graph_definition, line_delimiter );
    BOOST_FOREACH( const string& line, line_tokenizer )
    {
        // skip comment lines
        string decommented_line = strip_comments( line );
        if( decommented_line.empty() )
        {
            continue;
        }
        separator_t node_delimiter( " " );
        tokenizer_t node_tokenizer( decommented_line, node_delimiter );
        std::deque< string > nodes;

        BOOST_FOREACH( const string& s, node_tokenizer )
        {
            nodes.push_back( s );
        }
        if( nodes.size() < 2 )
        {
            throw std::runtime_error( ( boost::format(
                "Invalid line in graph definition, "
                "lines must have at minimum, two nodes. %1%" ) % line ).str() );
        }

        string parent_worker_name = nodes.front();
        nodes.pop_front();
        graph_ptr->add_worker( parent_worker_name );

        while( !nodes.empty() )
        {
            graph_ptr->add_worker_child( parent_worker_name, nodes.front() );
            graph_ptr->add_worker_parent( nodes.front(), parent_worker_name );
            nodes.pop_front();
        }
    }

    return graph_ptr;
}
////////////////////////////////////////////////////////////////////////////////

} //end graph
} //end kibitz
