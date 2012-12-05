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

#include <kibitz/publisher.hpp>

#include <boost/config.hpp>
#include <boost/thread.hpp>
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

#include <zmq.h>

#include <kibitz/kibitz_util.hpp>
#include <kibitz/messages/inproc_notification_message.hpp>
#include <kibitz/validator/validator.hpp>
#include <kibitz/locator/common.hpp>
#include <glog/logging.h>

using boost::dynamic_pointer_cast;

namespace ku = kibitz::util;
namespace k = kibitz;
namespace kg = kibitz::graph;
namespace kl = kibitz::locator;

struct graph_traversal_t {
  std::set<std::string> nodes;
  void operator()( const std::string& worker, const kg::node_ptr_t& ) {
    nodes.insert( worker );
  }
};

int test_main( int argc, char* argv[] )
{
  google::InitGoogleLogging(argv[0]);


  std::cout << "Testing graph validator" << std::endl;

  std::string graph_definition = "A B C\nB D\nC D";

  kg::worker_graph_ptr wgp = kg::create_worker_graph_from_string( graph_definition );
  BOOST_CHECK( wgp != NULL );
  graph_traversal_t traversor ;
  kg::node_ptr_t node = wgp->get_worker( "A" );
  BOOST_CHECK( node != NULL );
  node->traverse_in_edges( traversor );
  BOOST_CHECK( traversor.nodes.empty() );
  traversor.nodes.clear();
  node->traverse_out_edges( traversor );
  BOOST_CHECK( traversor.nodes.size() == 2 );
  BOOST_CHECK( traversor.nodes.count( "B" ) == 1 );
  BOOST_CHECK( traversor.nodes.count( "C" ) == 1 );

  wgp = kg::create_worker_graph_from_file( "../../config/test1.graph" );
  BOOST_CHECK( wgp != NULL );
  node = wgp->get_worker("D");
  BOOST_CHECK( node != NULL );
  BOOST_CHECK( node->get_in_edges().size() == 2 );
  BOOST_CHECK( node->get_out_edges().empty() );
  node = wgp->get_worker( "B" );
  BOOST_CHECK( node->get_in_edges().size() == 1 );
  BOOST_CHECK( node->get_out_edges().size() == 1 );
  std::cout << "Graph validator tests passed..." << std::endl;
  std::cout << "Testing binding generator" << std::endl;
  kl::binding_map_t bindings;
  kl::create_bindings( "tcp://192.168.1.2", wgp, 10000, bindings );  
  BOOST_CHECK( bindings.size() == 4 );
  std::cout << "Binding generator testing complete" << std::endl;

  std::string json = "{\"message_type\":\"notification\",\"version\":\"1.0\",\"notification_type\":\"heartbeat\","
    "\"worker_type\":\"A\",\"worker_id\":1,\"host\":\"foo.com\",\"process_id\":200000,\"ticks\":"
    "1000,\"port\":10000}";


  void* zmq_context = zmq_init(2);
  BOOST_CHECK( zmq_context );

  {
    std::cout << "Testing notification message bus" << std::endl;
        
    std::string publish_binding = "inproc://pub_binding";
    std::string inproc_binding = "inproc://inproc_binding";
    k::publisher publisher( zmq_context, publish_binding, ZMQ_PUB, inproc_binding );
    
    boost::thread test_thread_1( publisher );
    sleep(1);
    
    void* sub = ku::create_socket( zmq_context, ZMQ_SUB );

    zmq_connect( sub, publish_binding.c_str() );
    ku::check_zmq( zmq_setsockopt( sub, ZMQ_SUBSCRIBE, 0, 0 ) );

    publisher.send( json ) ;
   
    string response;
    ku::recv( sub, response );
    std::cout << "Got subscription response " << response << std::endl;
    BOOST_CHECK( json == response );
    k::inproc_notification_message stop_message( k::message::stop ) ;
    publisher.send( stop_message.to_json() );
    
    test_thread_1.join();
    std::cout << "Publisher test passed." << std::endl;
    std::cout << "Thread exited Ctl+C exits." << std::endl;
  }

  if( zmq_context ) {
    zmq_term( zmq_context );
  }




  return 0;
}
