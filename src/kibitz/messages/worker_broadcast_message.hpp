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
#ifndef worker_broadcast_message_hpp
#define worker_broadcast_message_hpp

#include <kibitz/messages/notification_message.hpp>

namespace kibitz
{

namespace notification
{
static const char* CREATE_BINDINGS = "create_bindings";
static const char* INITIALIZE_JOB = "initialize_job";
}

class worker_broadcast_message : public notification_message
{

    std::string notification_;
public:


    worker_broadcast_message( JSON::Object::Ptr json ) ;
    worker_broadcast_message( const std::string& notification ) ;
    virtual ~worker_broadcast_message() {}
    virtual std::string to_json() const;
    const std::string& notification() const
    {
        return notification_;
    }
};

typedef shared_ptr<worker_broadcast_message> worker_broadcast_message_ptr_t;
}

#endif
