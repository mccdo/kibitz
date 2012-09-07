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
#ifndef worker_query_response_hpp
#define worker_query_response_hpp

#include <kibitz/messages/notification_message.hpp>
#include <kibitz/messages/worker_info.hpp>

namespace kibitz {

  class KIBITZ_MESSAGE_EXPORT worker_query_response : public notification_message
  {
    worker_infos_t workers_;
  public :
    worker_query_response( ) ;
    worker_query_response( const ptree& json );
    ~worker_query_response();
    virtual string to_json() const ;
    const worker_infos_t& get_workers() const;
    const worker_infos_t& set_workers( const worker_infos_t& workers ) ;
    bool operator==( const worker_query_response& rval ) const; 
  };


  typedef shared_ptr< worker_query_response > worker_query_response_ptr_t;
}


#endif
