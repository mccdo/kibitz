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

#ifndef __COLLABORATION_MESSAGE_HPP__
#define __COLLABORATION_MESSAGE_HPP__

#include <kibitz/messages/message.hpp>
#include <kibitz/export_config.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

namespace kibitz
{


class KIBITZ_MESSAGE_EXPORT collaboration_message : public message
{
    std::string collaboration_type_;
    std::string job_id_;
protected:

    collaboration_message( const std::string& collaboration_type, const std::string& version = "1.0" )
        : message( "collaboration", version ),
          collaboration_type_( collaboration_type ),
        job_id_( lexical_cast< std::string>( boost::uuids::random_generator()() ) )
    {
    }

    collaboration_message( JSON::Object::Ptr json )
        : message( json )
    {
        get_value( json, "collaboration_type", collaboration_type_ );
        get_value( json, "job_id", job_id_ );
    }

    void populate_header( JSON::Object::Ptr json ) const
    {
        message::populate_header( json ) ;
        json->set( "collaboration_type", collaboration_type_ );
        json->set( "job_id", job_id_ );
    }

public:
    virtual ~collaboration_message() {}
    virtual std::string to_json() const = 0;
    virtual const std::string& collaboration_type() const
    {
        return collaboration_type_ ;
    }
    virtual const std::string& job_id() const
    {
        return job_id_;
    }
    virtual void set_job_id( const std::string& job_id )
    {
        job_id_ = job_id;
    }
};

typedef shared_ptr<collaboration_message> collaboration_message_ptr_t;
}

#endif



