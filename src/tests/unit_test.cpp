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
#include <kibitz/messages/collaboration_message_bundle.hpp>
#include <kibitz/messages/basic_collaboration_message.hpp>

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
using boost::dynamic_pointer_cast;

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
    std::string json;
    // TODO: fix me

    // kibitz::heartbeat heartbeat( command_line );
    // string json = heartbeat.to_json() ;
    // BOOST_CHECK( !json.empty() );
    // kibitz::message_ptr_t message = kibitz::message_factory( json );
    // BOOST_CHECK( message != NULL );
    // shared_ptr<kibitz::heartbeat> heartbeat_ptr = boost::dynamic_pointer_cast<kibitz::heartbeat>( message );
    // BOOST_CHECK( heartbeat_ptr != NULL );

    json = "{"
    "\"message_type\": \"collaboration\","
    "\"version\": \"1.0\","
    "\"collaboration_type\": \"collaboration_messages\","
    "\"job_id\": \"7abed30d-8e3f-446a-86b3-c14d462ab814\","
    "\"messages\":"
    "["
        "{"
           "\"message_type\": \"collaboration\","
            "\"version\": \"1.0\","
            "\"collaboration_type\": \"generic\","
            "\"job_id\": \"0437ccc4-6de2-466f-8cae-294213c54a12\","
            "\"payload\": \"520ee95c-6cfc-4306-984d-6c1138ac6af8\","
            "\"worker_type\": \"A\""
        "}"
      "]"
      "}";
    
    k::collaboration_message_bundle_ptr_t cbp = boost::static_pointer_cast<k::collaboration_message_bundle>( k::message_factory( json ) );
    BOOST_CHECK( cbp != NULL );
    BOOST_CHECK( cbp->messages().size() == 1 );
    return 0;
}
