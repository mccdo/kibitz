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
#ifndef in_edge_manager_hpp
#define in_edge_manager_hpp

#include <kibitz/common.hpp>
#include <kibitz/messages/notification_message.hpp>
#include <kibitz/kibitz_util.hpp>

namespace kibitz
{
/// \brief Handles messages coming from parent worker nodes
///
class in_edge_manager
{
    context& context_;
    const string worker_type_ ;
    const int worker_id_;
    util::sockman_ptr_t notification_socket_;

    void check_and_start_job( notification_message_ptr_t message ) ;
public:
    in_edge_manager( context& ctx );
    /// Copy c'tor - we provide this because we don't want to
    /// copy internal zmq sockets between threads
    in_edge_manager( const in_edge_manager& iem );
    virtual ~in_edge_manager() ;

    /// Thread functor
    void operator()() ;

    /// Used to communicate from other threads to
    /// the in edge manager
    ///
    /// \param json message to send to this thread
    ///
    void send_notification( const string& json ) ;


    static const char* NOTIFICATION_BINDING;

};
}

#endif
