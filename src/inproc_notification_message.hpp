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
#ifndef inproc_notification_message_hpp
#define inproc_notification_message_hpp


#include "notification_message.hpp"


namespace kibitz {



  class KIBITZ_MESSAGE_EXPORT inproc_notification_message : public notification_message {
    int notification_;
  public:
    inproc_notification_message( const ptree& json ) ;
    inproc_notification_message( int notification );
    virtual string to_json() const ;
    int get_notification() const { return notification_; }
  };

  typedef shared_ptr<inproc_notification_message> inproc_notification_ptr_t;
}

#endif

