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
#ifndef basic_collaboration_message_hpp
#define basic_collaboration_message_hpp

#include <kibitz/messages/collaboration_message.hpp>

#include <kibitz/export_config.hpp>

namespace kibitz
{
class KIBITZ_MESSAGE_EXPORT basic_collaboration_message : public collaboration_message
{
    string worker_type_;
    string payload_;

public:
    basic_collaboration_message( const string& worker_type, const string& payload ) ;

    basic_collaboration_message( JSON::Object::Ptr json ) ;

    virtual ~basic_collaboration_message() {}
    const string& worker_type() const
    {
        return worker_type_ ;
    }
    const string& payload() const
    {
        return payload_;
    }
    virtual string to_json( JSON::Object::Ptr json ) ;
    virtual string to_json() const ;



    static shared_ptr<basic_collaboration_message> create( JSON::Object::Ptr json ) ;

};

typedef shared_ptr<basic_collaboration_message> basic_collaboration_message_ptr_t;
}

#endif
