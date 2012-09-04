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
/*
#include "common.hpp"
#include "collaboration_message.hpp"
#include "notification_message.hpp"
*/

#include <string>
#include <vector>

namespace kibitz {
  typedef std::string payload_t;
  typedef std::vector< payload_t > collaboration_messages_t;
  typedef void ( *collaboration_callback )( const collaboration_messages_t& messages  );
  typedef void ( *initialization_callback )( void );

  struct context_information_t {
    std::string worker_type;
    std::string job_id;
    int worker_id;

  };

  void initialize( int argc, char* argv[] ) ; 
  void start() ;
  void terminate();
  void set_in_message_handler( collaboration_callback fn );
  void set_initialization_notification_handler( initialization_callback fn );
  void send_out_message( const payload_t& payload ) ;
  void send_notification_message( const payload_t& payload ) ;
  /**
   * Retrieves information about worker, for diagnostic purposes only
   */
  void get_context_information( context_information_t& context_information );
  

}

#endif
