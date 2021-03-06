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
#ifndef __CONTEXT_HPP__
#define __CONTEXT_HPP__


#include <kibitz/common.hpp>
#include <kibitz/bus.hpp>
#include <kibitz/messages/worker_status_message.hpp>
#include <kibitz/logging.hpp>

namespace kibitz
{

typedef std::vector< std::string > worker_types_t;

class context
{

    std::string worker_type_name_;
    std::string worker_id_;
    po::variables_map application_configuration_;
    void* zmq_context_;
    void* message_bus_socket_;
    bool signalled_;

    collaboration_callback inedge_message_handler_;
    initialization_callback initialization_handler_;
    bool status_publisher_enabled_;
    boost::mutex mutex_;
    std::string current_job_id_;

  boost::mutex collaboration_queue_lock_;
  int collaboration_queue_depth_;

  boost::mutex create_thread_lock_;
public:
    context( const po::variables_map& application_configuration );
    ~context();
    void set_worker_type( const std::string& worker_type_name ) ;
    void set_worker_id( const std::string& worker_id );

    const std::string& worker_type() const
    {
        return application_configuration_["worker-type"].as<std::string>();
    }
    int worker_id()
    {
        return application_configuration_["worker-id"].as<int>() ;
    }

    void register_initialization_notification_handler( initialization_callback initialization_handler );
    void register_inedge_message_handler( collaboration_callback inedge_message_handler ) ;
    collaboration_callback get_inedge_message_handler() ;
    initialization_callback get_initialization_notification_callback() ;
    void start() ;
    void stop() ;
    void terminate();
    void* zmq_context() ;
    void send_internal_message( const char* message ) ;

    const po::variables_map& get_config() const;


    void send_out_message( const std::string& payload ) ;
    void send_notification_message( const std::string& payload );
    void set_job_id( const std::string& job_id ) ;
    void get_job_id( std::string& job_id );
    void send_worker_status( worker_status_t status )  ;
    int increment_collaboration_queue() ;
    int decrement_collaboration_queue() ;
    int get_collaboration_queue_size()  ;

  template< typename F > 
  boost::thread* create_thread( F threadfunc ) {
    boost::thread* pthrd = NULL;
    {
      boost::mutex::scoped_lock lock( create_thread_lock_ );
      pthrd = m_threads.create_thread( threadfunc );
    }
    return pthrd;
  }
  
private:
    ///Logger utilities for debugging
    Poco::Logger& m_logger;
    ///Logger utilities for debugging
    LogStreamPtr m_logStream;
    ///The thread group that maintains all of the threads for this context. This
    ///member is used to interrupt all of the threads from an external signal.
    boost::thread_group m_threads;


};

typedef shared_ptr< context > context_ptr;


}


#endif
