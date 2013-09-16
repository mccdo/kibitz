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
#ifndef __HEARTBEAT_HPP__
#define __HEARTBEAT_HPP__


#include <kibitz/export_config.hpp>

#include <kibitz/messages/notification_message.hpp>

namespace kibitz
{

class worker_notification_message;

  /// \brief Sent periodically to indicate aliveness of locator
  ///
class KIBITZ_MESSAGE_EXPORT heartbeat : public notification_message
{
    string host_name_;
    int pid_;
    int port_;
    int ticks_;
    friend class worker_notification_message;
public :
    heartbeat( int port ) ;
    heartbeat( JSON::Object::Ptr json  );
    virtual ~heartbeat() ;
    virtual string to_json() const ;

    void increment_tick_count();
};

typedef shared_ptr<heartbeat> heartbeat_ptr_t;

}


#endif

