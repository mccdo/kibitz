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
#ifndef registry_hpp
#define registry_hpp

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <kibitz/messages/heartbeat.hpp>
#include <kibitz/export_config.hpp>

/**
 *  This object aggregates heartbeats from workers and rebroadcasts
 *  discovery information to other workers.
 */
class KIBITZ_LOCATOR_EXPORT registry
{
    typedef boost::shared_ptr<kibitz::heartbeat> heartbeat_ptr_t;
    typedef std::map< int, heartbeat_ptr_t > worker_map_t;
    typedef std::map< std::string, worker_map_t > worker_type_map_t;


    void* publisher_socket_;
    void* inproc_pub_socket_;
    void* inproc_sub_socket_;


    bool one_second_elapsed( boost::posix_time::ptime& last_send );

public:
    registry( void* inproc_pub_socket, void* inproc_sub_socket, void* publisher_socket );
    virtual ~registry() ;
    void push_message( const std::string& msg ) ;
    void push_message( const kibitz::message& message );
    /**
     *  Thread function
     */
    void operator()() ;


};

#endif
