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
#ifndef job_initialization_message_hpp
#define job_initialization_message_hpp

#include <kibitz/messages/notification_message.hpp>

namespace kibitz
{
namespace notification
{
static const char* JOB_INITIALIZATION = "job_initialization";
}
/**
 *  Message used to start jobs.
 */
class job_initialization_message : public notification_message
{
    string worker_type_;
    int worker_id_;
public:
    job_initialization_message( const ptree& json ) ;
    job_initialization_message( const string& worker_type, int worker_id );
    virtual ~job_initialization_message( ) {}
    const string& worker_type() const
    {
        return worker_type_;
    }
    const int worker_id() const
    {
        return worker_id_ ;
    }

    virtual string to_json() const ;
};

typedef shared_ptr<job_initialization_message> job_initialization_message_ptr_t ;
}


#endif
