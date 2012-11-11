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
#include <kibitz/bus.hpp>
#include <kibitz/kibitz_util.hpp>


namespace kibitz
{

bus::bus( void* zmq_context, const char* binding, int sock_type )
    : socket_( NULL )
{
    socket_ = util::create_socket( zmq_context, sock_type );

}

bus::~bus()
{
    util::close_socket( socket_ );

}

void bus::close()
{
    util::close_socket( socket_ ) ;
    socket_ = NULL;
}

pub::pub( void* zmq_context, const char* binding )
    : bus( zmq_context, binding, ZMQ_PUB )
{
    util::check_zmq( zmq_bind( socket_, binding ) );
}

void pub::send( const string& json )
{
    DLOG( INFO ) << "publishing " <<  json;
    util::send( socket_, json );
}

sub::sub( void* zmq_context, const char* binding )
    : bus( zmq_context, binding, ZMQ_SUB )
{
    util::check_zmq( zmq_connect( socket_, binding ) );
    util::check_zmq( zmq_setsockopt( socket_, ZMQ_SUBSCRIBE, "", 0 ) );
}

  void sub::recv( string& message ) {
    util::recv( socket_, message );
  }

}
