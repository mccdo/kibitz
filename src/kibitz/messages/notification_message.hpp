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
#ifndef notification_message_hpp
#define notification_message_hpp

#include <kibitz/export_config.hpp>

#include <kibitz/messages/message.hpp>


namespace kibitz
{

class KIBITZ_MESSAGE_EXPORT notification_message : public message
{
    string notification_type_;
protected:
    notification_message( const string& notification_type, const string& version = "1.0" )
        : message( "notification", version ),
          notification_type_( notification_type ) {}
    notification_message( JSON::Object::Ptr json ) 
      : message( json ) {
      get_value( json, "notification_type", notification_type_ );
    }
    void populate_header( JSON::Object::Ptr json  ) const
    {
        message::populate_header( json );
        json->set( "notification_type", notification_type_ );
    }
public :
    virtual ~notification_message() {}
    virtual string to_json() const = 0;
    const string& notification_type() const
    {
        return notification_type_ ;
    }
};

typedef shared_ptr<notification_message> notification_message_ptr_t;

}


#endif


