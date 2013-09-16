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
#include <kibitz/messages/collaboration_message_bundle.hpp>
#include <kibitz/messages/basic_collaboration_message.hpp>
#include <kibitz/messages/binding_notification.hpp>

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
   
    

    
  JSON::Object::Ptr parsed;
  kibitz::read_json( json, parsed );
  BOOST_CHECK( parsed->size()  );
  
  Dynamic::Var message_type = parsed->get("message_type");
  BOOST_CHECK( message_type.toString() == "collaboration" );

  Dynamic::Var missing_type = parsed->get("not_present" );
  BOOST_CHECK( missing_type.isEmpty() );

  string t;
  kibitz::get_value( parsed, "message_type", t );
  BOOST_CHECK( t == "collaboration" );

  k::collaboration_message_bundle_ptr_t cbp = boost::static_pointer_cast<k::collaboration_message_bundle>( k::message_factory( json ) );
  BOOST_CHECK( cbp->messages().size() == 1 );
  std::cout << "job id = " << cbp->job_id() << std::endl;
  BOOST_CHECK( cbp->job_id() == "7abed30d-8e3f-446a-86b3-c14d462ab814" );
  BOOST_CHECK( cbp->messages().front()->job_id() == "0437ccc4-6de2-466f-8cae-294213c54a12" );
  BOOST_CHECK( cbp->messages().front()->worker_type() == "A" );
  BOOST_CHECK( cbp->messages().front()->payload() == "520ee95c-6cfc-4306-984d-6c1138ac6af8" );
  

  try {
    kibitz::get_value( parsed, "xxxxx", t );
    BOOST_CHECK( false );
  } catch( const std::runtime_error& e ) {
    std::cout << "Caught expected exception -> " << e.what() << std::endl;
  }

  try {
    string empty_json = "";
    JSON::Object::Ptr empty_parsed;
    kibitz::read_json( empty_json, empty_parsed );
    BOOST_CHECK( false );
  } catch( const JSON::JSONException& e ) {
    std::cout << "Caught expected exception -> " << e.what() << std::endl;
  }


  json = cbp->to_json();
  cbp = boost::static_pointer_cast<k::collaboration_message_bundle>( k::message_factory( json ) );
  
  BOOST_CHECK( cbp->messages().size() == 1 );
  std::cout << "job id round 2 = " << cbp->job_id() << std::endl;
  BOOST_CHECK( cbp->job_id() == "7abed30d-8e3f-446a-86b3-c14d462ab814" );
  BOOST_CHECK( cbp->messages().front()->job_id() == "0437ccc4-6de2-466f-8cae-294213c54a12" );
  BOOST_CHECK( cbp->messages().front()->worker_type() == "A" );
  BOOST_CHECK( cbp->messages().front()->payload() == "520ee95c-6cfc-4306-984d-6c1138ac6af8" );
  
  json = "{"
    "\"message_type\": \"notification\","
    "\"version\": \"1.0\","
    "\"notification_type\": \"heartbeat\","
    "\"host\": \"vagrant-ubuntu-quantal-64\","
    "\"process_id\": 24381,"
    "\"port\": 5556,"
    "\"ticks\": 35"
     "}";
  k::heartbeat_ptr_t hbp = boost::static_pointer_cast<k::heartbeat>( k::message_factory( json ) );
  BOOST_CHECK( hbp->version() == "1.0" );
  BOOST_CHECK( hbp->notification_type() == "heartbeat" );
  BOOST_CHECK( hbp->host() == "vagrant-ubuntu-quantal-64" );
  BOOST_CHECK( hbp->process_id() == 24381 );
  BOOST_CHECK( hbp->port() == 5556 );
  BOOST_CHECK( hbp->ticks() == 35 );
  json = hbp->to_json() ;
  hbp = boost::static_pointer_cast<k::heartbeat>( k::message_factory( json ) );
  BOOST_CHECK( hbp->version() == "1.0" );
  BOOST_CHECK( hbp->notification_type() == "heartbeat" );
  BOOST_CHECK( hbp->host() == "vagrant-ubuntu-quantal-64" );
  BOOST_CHECK( hbp->process_id() == 24381 );
  BOOST_CHECK( hbp->port() == 5556 );
  BOOST_CHECK( hbp->ticks() == 35 );

  json = "{"
    "\"message_type\": \"notification\","
    "\"version\": \"1.0\","
    "\"notification_type\": \"binding\","
    "\"target_worker\": \"A\","
    "\"zmq_binding\": \"tcp://localhost:6000\""
    "}";

  k::binding_notification_ptr_t bnp = boost::static_pointer_cast<k::binding_notification>(k::message_factory( json ) );
  BOOST_CHECK( bnp->binding() == "tcp://localhost:6000" );
  BOOST_CHECK( bnp->target_worker() == "A" );
  BOOST_CHECK( bnp->notification_type() == "binding" );
  json = bnp->to_json() ;
  bnp = boost::static_pointer_cast<k::binding_notification>(k::message_factory( json ) );
  
  BOOST_CHECK( bnp->binding() == "tcp://localhost:6000" );
  BOOST_CHECK( bnp->target_worker() == "A" );
  BOOST_CHECK( bnp->notification_type() == "binding" );

  return 0;
}
