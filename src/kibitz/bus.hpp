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
#ifndef bus_hpp
#define bus_hpp

#include <kibitz/common.hpp>
#include <kibitz/logging.hpp>

namespace kibitz
{

class bus
{

protected:
    void* socket_;
    bus( void* zmq_context, const char* binding, int sock_type ) ;
    virtual ~bus();
public:
    void* socket()
    {
        return socket_;
    }
    void close() ;
protected:
    ///Logger utilities for debugging
    Poco::Logger& m_logger;
    ///Logger utilities for debugging
    LogStreamPtr m_logStream;
};

class pub : public bus
{
public:
    pub( void* zmq_context, const char* binding );
    virtual ~pub() {}
    void send( const string& json );
};

class sub : public bus
{
public:
    sub( void* zmq_context, const char* binding ) ;
    virtual ~sub() {}


};

}

#endif
