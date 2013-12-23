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
#ifndef worker_notification_hpp
#define worker_notification_hpp


#include <kibitz/messages/notification_message.hpp>


namespace kibitz
{


/// \brief Message used when a worker sends a notification message
///
/// These messages are intended to contain a payload that is sent to
/// agents that are external to kibitz. The json message is stripped and
/// only the payload is sent to external agents.
class KIBITZ_MESSAGE_EXPORT worker_notification : public notification_message
{
    std::string payload_;
public:
    worker_notification( JSON::Object::Ptr json ) ;
    worker_notification( const std::string& payload );
    virtual std::string to_json() const ;
    const std::string& get_payload() const
    {
        return payload_;
    }

    static const char* NOTIFICATION_TYPE;
};

typedef shared_ptr<worker_notification> worker_notification_ptr_t;
}

#endif

