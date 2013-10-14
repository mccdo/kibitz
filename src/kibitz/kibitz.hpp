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
#ifndef __KIBITZ_HPP__
#define __KIBITZ_HPP__
/// \file kibitz.hpp
/// Contains callback functions and declarations that are used to implement
/// workers.  This header exposes the functionality of the kibitz 
/// library to developers. 
/// 
/// \mainpage Kibitz Model Integration Framework
///
/// \section introduction Introduction
/// <p>Kibitz is a messaging based framework that makes it easy to build software components
/// that collaborate with one another.  Workers are grouped into different worker
/// types.  Worker types can be thought of as nodes in a directed graph with each
/// worker type getting messages from one or more parent worker types, performing work
/// and then passing messages containing the result of this work to child workers.  The
/// graph is defined in a configuration file.</p>
/// <p>The framework is comprised of a number of components.  The worker-locator routes
/// messages between workers according to a graph file passed on start up.  Developers 
/// link to the kibitz library and implement call back functions to create different
/// worker types and implement custom tasks. </p> 
///
/// \section worker_locator Worker Locator
/// <p>The worker locator keeps track of workers and routes messages between them.  This
/// routing is defined by a graph file passed on the command line.  Kibitz 
/// uses Google GLog Logging Framework http://code.google.com/p/google-glog/ so logging
/// is controlled by GLOG_ environment variables.   
/// 
/// \code
/// worker-locator:
///   -h [ --help ]                         Show help message
///   -p [ --port ] arg (=5556)             Port used by locator to distribute 
///                                         heartbeats.
///   -l [ --listen-port ] arg (=5557)      Port listens for messages from workers 
///                                         and control scripts.
///   -t [ --context-threads ] arg (=4)     zmq context thread count
///   -d [ --daemon ]                       Run as a daemon
///   -f [ --graph-definition-file ] arg    File containing collaboration graph 
///                                         definition
///   --pid-file arg (=/var/run/kibitz-locator.pid)
///                                         Location of pid file for daemon mode
///   --heartbeat-frequency arg (=100)      Heartbeat frequency in milliseconds
///   -H [ --host ] arg                     Host name or IP address that workers 
///                                         will connect to for collaboration 
///                                         messages.
///   -P [ --base-port ] arg (=6000)        Start of port range that workers will 
///                                         bind to for collaboration messages
/// \endcode
/// \section worker Worker
/// <p>Workers are units of business logic that are implemented by developers using the Kibitz
/// library.  Developers implement various callbacks found in kibitz.hpp. The kibitz::collaboration_callback 
/// function is implemented to handle messages sent from parent workers. This callback is registered 
/// with the kibitz framework by passing it to the kibitz::set_in_message_handler function. 
/// Since workers can have multiple parent workers
/// the callback will not be invoked until all parent worker types have sent messages.  Only then
/// is the callback invoked and passed all messages from parent workers.
/// kibitz::send_out_message must be called by developers implementing kibitz::collaboration_callback
/// if the worker is to send messages to child workers.  If the worker is a terminal worker i.e. doesn't
/// have and chid workers, kibitz::send_notification_message may be called to send messages to 
/// external agents on a port defined by the --notification-port command line parameter.</p> 
/// <p>Initial worker types, in other words, workers without parents must implement 
/// a kibitz::initialization_callback which will be invoked when a kibitz::job_initialization_message
/// is sent to the worker-locator on it's --listen-port. The purpose of this feature is to start the job, 
/// and perform any initialization. </p>
/// \subsection worker_type Worker Type
/// Each worker with distinct functionality must be named a unique worker type defined 
/// by the --worker-type parameter on the command line.  Note that there can be multiple
/// instances of the same worker type but each of these must have a unique --worker-id.  
/// If multiple processes are running the same worker type messages are passed to each instance 
/// in turn using a round robin.  In this way kibitz can acheive redundancy and/or scale out
/// to handle load.   
/// \subsection worker_lifecycle Worker Lifecycle 
/// 
/// <ol>
/// <li>kibitz::initialize</li>
/// <li>Register callbacks kibitz::set_in_message_handler or 
/// kibitz::set_initialization_notification_handler</li>
/// <li>kibitz::start</li>
/// <li>Call kibitz::send_out_message or kibitz::send_notification_message inside 
/// callbacks</li>
/// <li>kibitz::terminate</li>
/// </ol>
/// 
/// \subsection worker_command_line Worker Command Line
/// \code
/// "worker" options:
///  -h [ --help ]                         Show help message
///  -I [ --worker-id ] arg                (Required) Integer that identifies 
///                                        worker. Must be unique with worker type
///  -T [ --worker-type ] arg              (Required) Name of the type of worker.
///  -L [ --locator-host ] arg             IP Address or DNS name of locator
///  -R [ --locator-receive-port ] arg (=5556)
///                                        Port to receive notifications from 
///                                        locator
///  -S [ --locator-send-port ] arg (=5557)
///                                        Port to send messages to locator
///  -P [ --notification-port ] arg        Optional port to publish notification 
///                                        messages
///  -t [ --context-threads ] arg (=2)     Thread count passed to zmq_init
/// \endcode
/// \section graph_file Graph Definition File
/// A directed graph is used to define the trie that messages traverse
/// as they are passed from worker type to worker type.  This graph is 
/// defined in a file that is passed to the worker-locator using the
/// --graph-definition-file command line argument.  The graph file
/// is formatted with a the name of a parent worker defined as the first
/// token on a line, followed by children worker types. For example:
/// \code
/// parent child1 child2
/// \endcode
/// Multiple lines containing parent child relationships form the definition of the 
/// entire graph.  In this example worker_a is the root sending a message to both
/// worker_b and worker_c. worker_d is the terminal node with two in edges
/// receiving messages from both worker_b and worker_c. Of course there may 
/// be multiple instances of each worker type, for example we might have
/// five instances of worker_b with id's 1, 2, 3, 4 and 5 to distibute load, 
///  but we do not have to define these instances in this file.
/// \code
/// worker_a worker_b worker_c
/// worker_b worker_d
/// worker_c worker_d
/// \endcode 



#include <kibitz/export_config.hpp>

#include <string>
#include <vector>

#include <boost/program_options.hpp>

namespace kibitz
{
typedef std::string payload_t;
typedef std::vector< payload_t > collaboration_messages_t;

  /// Signature for callback function to handle messages from other workers. 
typedef void ( *collaboration_callback )( const collaboration_messages_t& messages );

/// Signature for callback function that can handle
/// job initialization messages if registered with kibitz::set_initialization_notification_handler
///
typedef void ( *initialization_callback )( const payload_t&  );

struct context_information_t
{
    std::string worker_type;
    std::string job_id;
    int worker_id;

};

///Command line options to control kibitz
KIBITZ_EXPORT boost::program_options::options_description get_command_line_description();

/// Must be called to initialize a worker that implements the kibitz
/// library. This method enables 3rd party applications to implement
/// their own command line parsing.
KIBITZ_EXPORT void initialize( boost::program_options::variables_map& command_line );

/// Must be called to initialize a worker that implements the kibitz
/// library
KIBITZ_EXPORT void initialize( int argc, char* argv[] ) ;

/// Starts listener threads that handle messages, call this method
/// after callbacks are registered.
KIBITZ_EXPORT void start() ;

/// Cleans up resources used by library
KIBITZ_EXPORT void terminate();

  /// Used to define function that will handle messages received
  /// from predecessor workers
  /// \param fn callback function containing worker functionality
  ///
KIBITZ_EXPORT void set_in_message_handler( collaboration_callback fn );

  /// Used to register a callback to initialize a job. This is defined in root workers 
  /// (workers without parent workers).
  ///
  /// \param fn pointer to kibitz::initialization_callback
  /// 
KIBITZ_EXPORT void set_initialization_notification_handler( initialization_callback fn );

  /// This method should be called from within the implementation of kibitz::collaboration_callback
  /// and will send a message to child workers.
  ///  
  /// \param payload The contents of the message that will be sent to 
  /// child workers
  /// 
KIBITZ_EXPORT void send_out_message( const payload_t& payload ) ;

  /// Sends a message to listeners on optional --notification-port
  /// 
  /// \param payload a string that will be sent to listeners.
  ///
KIBITZ_EXPORT void send_notification_message( const payload_t& payload ) ;

  /// Retrieves information about worker, for diagnostic purposes only
KIBITZ_EXPORT void get_context_information( context_information_t& context_information );


}

#endif
