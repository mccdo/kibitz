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
#include <kibitz/locator/notification_message_bus.hpp>


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
#include <kibitz/locator/notification_message_bus.hpp>
#include <kibitz/kibitz_util.hpp>

using boost::dynamic_pointer_cast;

namespace ku = kibitz::util;

int test_main( int argc, char* argv[] )
{

  std::string json = "{\"message_type\":\"notification\",\"version\":\"1.0\",\"notification_type\":\"heartbeat\","
    "\"worker_type\":\"A\",\"worker_id\":1,\"host\":\"foo.com\",\"process_id\":200000,\"ticks\":"
    "1000,\"publisher_ports\":{\"B\":10001,\"C\":10002}}";


  // void* zmq_context = zmq_init(2);
  // BOOST_CHECK( zmq_context );
  // {
  //   std::cout << "Testing notification message bus" << std::endl;
  //   std::string inproc_publish_binding = "inproc:://inproc_publish_binding";
  //     locator::notification_message_bus notification_message_bus( zmq_context, inproc_publish_binding ); 
  //     boost::thread test_thread_1( notification_message_bus );
  
  //     void* socket = ku::create_socket(  zmq_context, ZMQ_SUB );
  //     zmq_connect( socket, inproc_publish_binding.c_str() );
  //     ku::check_zmq( zmq_setsockopt( socket, ZMQ_SUBSCRIBE, "", 0 ) );

  //     locator::send_notification_message( zmq_context, json );
  //     std::string response;
  //     ku::recv_async( socket, response ) ;
  //     BOOST_CHECK( json == response );
  //     ku::close_socket( socket );
  // }

  // if( zmq_context ) {
  //   zmq_term( zmq_context );
  // }


  return 0;
}
