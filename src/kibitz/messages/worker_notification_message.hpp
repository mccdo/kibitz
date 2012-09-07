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
#ifndef worker_notification_message_hpp
#define worker_notification_message_hpp

#include <kibitz/export_config.hpp>

#include <kibitz/messages/heartbeat.hpp>


namespace kibitz {

  class KIBITZ_MESSAGE_EXPORT worker_notification_message : public notification_message {
    string worker_type_;
    int32_t worker_id_;
    string host_name_;
    int16_t port_;


  public:
    worker_notification_message( const ptree& json ) ;
    worker_notification_message( heartbeat_ptr_t hb );
    worker_notification_message( const string& worker_type );
    virtual ~worker_notification_message() {}
    const string& worker_type() const ;
    const string& host_name() const;
    int32_t worker_id() const;
    int16_t port() const ;
    virtual string to_json() const;    

    
  };

  typedef shared_ptr<worker_notification_message> worker_notification_message_ptr_t;

}
#endif
