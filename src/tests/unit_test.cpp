/*************** <auto-copyright.rb BEGIN do not edit this line> **************
 *
 * Copyright 2012-2012 by Ames Laboratory
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *************** <auto-copyright.rb END do not edit this line> ***************/
#include <kibitz/common.hpp>

#include <kibitz/messages/heartbeat.hpp>
#include <kibitz/messages/worker_query_response.hpp>
#include <kibitz/messages/worker_query.hpp>
#include <kibitz/validator/worker_graph.hpp>

#include <boost/config.hpp>
#ifdef BOOST_WINDOWS
# pragma warning(disable: 4273)
#endif
#include <boost/test/minimal.hpp>
#ifdef BOOST_WINDOWS
# pragma warning(default: 4273)
#endif

#include <fstream>
#include <vector>
#include <iostream>

#include <yaml-cpp/yaml.h>

namespace k = kibitz;
namespace kg = kibitz::graph;
using boost::dynamic_pointer_cast;


struct teststruct_t {
  std::set<string> workers;
  void operator()( const string& worker_name, const kg::node_ptr_t&  ) {
    workers.insert( worker_name );
  }
};



int test_main( int argc, char* argv[] )
{

  po::options_description options( "test" );
  options.add_options()
    ( "help,h", "Show help message" )
    ( "worker-id,I", po::value< int >()->default_value( 1 ), "xxx" )
    ( "worker-type,T", po::value< string >()->default_value( "test-worker" ), "(Required) Name of the type of worker." )
    ( "multicast-binding,b", po::value<string>()->default_value( "xxx" ), "(Required) zmq epgm multicast binding" )
    ( "publish-port,p", po::value<int>()->default_value( 8999 ), "Port for tcp based zmq messages" )
    ( "notification-port,P", po::value<int>()->default_value( 9999 ), "port to publish notification messages" )
    ( "context-threads,t", po::value< int >()->default_value( 1 ), "Thread count passed to zmq_init" )
    ;
  po::variables_map command_line;
  po::store( po::parse_command_line( argc, argv, options ), command_line );
  po::notify( command_line );


  std::string json = "{\"message_type\":\"notification\",\"version\":\"1.0\",\"notification_type\":\"heartbeat\","
                      "\"worker_type\":\"A\",\"worker_id\":1,\"host\":\"foo.com\",\"process_id\":200000,\"ticks\":"
                      "1000,\"publisher_ports\":{\"B\":10001,\"C\":10002}}";
  kibitz::message_ptr_t message = kibitz::message_factory( json );
  BOOST_CHECK( message != NULL );
  shared_ptr<kibitz::heartbeat> heartbeat_ptr = boost::dynamic_pointer_cast<kibitz::heartbeat>( message );  
  BOOST_CHECK( heartbeat_ptr != NULL );
  BOOST_CHECK( heartbeat_ptr->get_publishers().count( "B" ) == 1 );
  BOOST_CHECK( heartbeat_ptr->get_publishers().find( "B" )->second == 10001 );
  BOOST_CHECK( heartbeat_ptr->get_publishers().count( "C" ) == 1 );
  BOOST_CHECK( heartbeat_ptr->get_publishers().find("C")->second == 10002 );
  BOOST_CHECK( heartbeat_ptr->get_publishers().count("A") == 0 );
  heartbeat_ptr->set_publisher( "D", 10003 );
  heartbeat_ptr = boost::dynamic_pointer_cast<kibitz::heartbeat>( kibitz::message_factory( heartbeat_ptr->to_json() ) );
  BOOST_CHECK( heartbeat_ptr->get_publishers().find("D")->second == 10003 );
  BOOST_CHECK( heartbeat_ptr->get_publishers().size() == 3 );

  kibitz::heartbeat heartbeat( command_line );
  json = heartbeat.to_json() ;
  BOOST_CHECK( !json.empty() );
  message = kibitz::message_factory( json );
  BOOST_CHECK( message != NULL );
  heartbeat_ptr = boost::dynamic_pointer_cast<kibitz::heartbeat>( message );
  BOOST_CHECK( heartbeat_ptr != NULL );

  

  std::vector< string > edges;
  string scalar;
  std::fstream stm( "../../config/test-worker-b.yaml" );
  YAML::Parser parser( stm );
  YAML::Node doc ;
  parser.GetNextDocument( doc );
  YAML::Iterator it = doc.begin();
  it.first() >> scalar;
  it.second() >> edges;
  BOOST_CHECK( scalar == "in-edges" );
  BOOST_CHECK( edges.size() == 2 );
  ++it;
  BOOST_CHECK( it == doc.end() );

  k::worker_infos_t workers;
  k::worker_info worker;
  worker.worker_type = "test-worker-a";
  worker.worker_id = 1;
  worker.port = 9000;
  worker.host = "foo.com";
  workers.push_back( worker );
  k::worker_query_response response;
  response.set_workers( workers );
  json = response.to_json();
  std::cout << "worker query response " << json << std::endl;
  BOOST_CHECK( !json.empty() );
  k::worker_query_response_ptr_t worker_query_response_ptr = dynamic_pointer_cast< k::worker_query_response >( k::message_factory( json ) );
  BOOST_CHECK( worker_query_response_ptr != NULL );
  BOOST_CHECK( response == *worker_query_response_ptr );

  worker.worker_type = "test-worker-a";
  worker.worker_id = 2;
  worker.port = 9001;
  worker.host = "foo.com";
  workers.push_back( worker );
  response.set_workers( workers );
  std::cout << "with two workers " << response.to_json() << std::endl;
  worker_query_response_ptr = dynamic_pointer_cast< k::worker_query_response> ( k::message_factory( response.to_json() ) );
  BOOST_CHECK( worker_query_response_ptr != NULL );
  BOOST_CHECK( response == *worker_query_response_ptr );

  k::worker_query wq( "worker-type-a" );
  json = wq.to_json();
  std::cout << "worker query " << json << std::endl;
  BOOST_CHECK( !json.empty() );
  k::worker_query_ptr_t worker_query_ptr = dynamic_pointer_cast<k::worker_query>( k::message_factory( json ) );
  BOOST_CHECK( worker_query_ptr != NULL );
  BOOST_CHECK( worker_query_ptr->worker_type() == "worker-type-a" );


  string graph_definition = "A B C\nB D\nC D";
  kg::worker_graph_ptr wgp = kg::create_worker_graph_from_string( graph_definition );
  BOOST_CHECK( wgp != NULL );
  teststruct_t st;
  kg::node_ptr_t ptr = wgp->get_worker( "A" );
  BOOST_CHECK( ptr != NULL );
  ptr->traverse_in_edges( st );
  BOOST_CHECK( st.workers.empty() );
  st.workers.clear();
  ptr->traverse_out_edges( st );
  BOOST_CHECK( st.workers.size() == 2 );
  BOOST_CHECK( st.workers.count( "C" ) == 1 );
  BOOST_CHECK( st.workers.count( "B" ) == 1 );
  st.workers.clear();
  ptr = wgp->get_worker( "C" );
  BOOST_CHECK( ptr != NULL );
  ptr->traverse_in_edges( st );
  BOOST_CHECK( st.workers.size() == 1 );
  BOOST_CHECK( st.workers.count("A") == 1 );
  st.workers.clear();
  ptr = wgp->get_worker( "D") ;
  ptr->traverse_in_edges( st );
  BOOST_CHECK( st.workers.size() == 2 );
  BOOST_CHECK( st.workers.count("C") == 1 );
  BOOST_CHECK( st.workers.count("B") == 1 );
  st.workers.clear();
  ptr->traverse_out_edges( st );
  BOOST_CHECK( st.workers.empty() );

  std::cout << "create worker graph from file" << std::endl;
  wgp = kg::create_worker_graph_from_file( "../../config/test1.graph" );
  BOOST_CHECK( wgp != NULL );
  ptr = wgp->get_worker( "A" );
  BOOST_CHECK( ptr != NULL );
  st.workers.clear();
  ptr->traverse_out_edges( st ) ;
  BOOST_CHECK( st.workers.size() == 2 );
  BOOST_CHECK( st.workers.count( "B" ) == 1 );
  BOOST_CHECK( st.workers.count( "C" ) == 1 );
  ptr = wgp->get_worker( "B" );
  BOOST_CHECK( ptr != NULL );
  st.workers.clear();
  ptr->traverse_in_edges( st );
  BOOST_CHECK( st.workers.size() == 1);
  BOOST_CHECK( st.workers.count( "A" ) == 1 );
  st.workers.clear() ;
  ptr->traverse_out_edges( st ) ;
  BOOST_CHECK( st.workers.count( "D" ) == 1 );
  
    

  return 0;
}
