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
#ifndef worker_query_hpp
#define worker_query_hpp

#include <kibitz/export_config.hpp>

#include <kibitz/messages/notification_message.hpp>

namespace kibitz
{
class KIBITZ_MESSAGE_EXPORT worker_query : public notification_message
{
    const string worker_type_;
public:
    worker_query( const string& worker_type ) ;
    worker_query( const ptree& tree ) ;
    virtual ~worker_query();
    const string& worker_type() const
    {
        return worker_type_;
    }

    virtual string to_json() const;
};

typedef shared_ptr<worker_query> worker_query_ptr_t;
}

#endif
