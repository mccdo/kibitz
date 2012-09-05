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
#ifndef worker_map_hpp
#define worker_map_hpp


#include "kibitz.hpp"
#include "context.hpp"
#include "kibitz_util.hpp"
#include "worker_notification_message.hpp"
#include "worker_query.hpp"
#include "worker_query_response.hpp"
#include "heartbeat.hpp"
#include "export_config.hpp"

#include <map>

namespace kibitz {
  
  class KIBITZ_EXPORT worker_map {
    typedef int worker_id_t;
    typedef string worker_type_t;
    typedef std::map< worker_id_t, worker_notification_message_ptr_t > worker_by_id_t;
    typedef std::map< string, worker_by_id_t > worker_map_t;
    void* send_socket_;
    context* context_;

    worker_map( void* send_socket );
    void handle_worker_notification( message_ptr_t message, worker_map_t& workers ) ;
    worker_infos_t process_query( const string& worker_type, const worker_map_t& workers ) const; 
  public:
    worker_map(context* context) ;
    virtual ~worker_map();
    void operator()();
    void send_worker_notification_from_heartbeat( const string& json ) ;
    /**
     * Retrieves connection information for inedge workers
     */
    worker_infos_t get_in_edge_workers( const string& worker_type ) const ;
    static shared_ptr<worker_map> get_worker_map( void* context );
    
  };

  typedef shared_ptr<worker_map> worker_map_ptr_t;
}

#endif
