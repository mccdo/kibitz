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

#include <kibitz/message_base.hpp>
#include <kibitz/kibitz_util.hpp>
#include <kibitz/kibitz.hpp>


using kibitz::util::create_socket;
using kibitz::util::close_socket;
using kibitz::util::check_zmq;

namespace kibitz
{

////////////////////////////////////////////////////////////////////////////////
message_base::message_base( context* context )
    :
    shutdown_( new bool( false ) ),
    context_( context ),
    m_logger( Poco::Logger::get("message_base") )
{
    m_logStream = LogStreamPtr( new Poco::LogStream( m_logger ) );
}
////////////////////////////////////////////////////////////////////////////////
message_base::~message_base()
{
    //delete shutdown_;
}
////////////////////////////////////////////////////////////////////////////////
bool message_base::shutdown() const
{
    return *shutdown_;
}
////////////////////////////////////////////////////////////////////////////////
void message_base::internal_command_handler()
{
    KIBITZ_LOG_NOTICE( "Created internal thread message handler" );
    void* socket = NULL;
    try
    {
        socket = create_socket( context_->zmq_context(), ZMQ_SUB );
        check_zmq( zmq_connect( socket, INPROC_COMMAND_BINDING ) );
        check_zmq( zmq_setsockopt( socket, ZMQ_SUBSCRIBE, "", 0 ) );

        while( true )
        {
            string message;
            kibitz::util::recv( socket, message );
            KIBITZ_LOG_DEBUG( "Message bus received " << message );

            if( message == SHUTDOWN_MSG )
            {
                KIBITZ_LOG_NOTICE( "setting shutdown flag" );
                *shutdown_ = true;
                break;
            }
        }
    }
    catch( const util::queue_interrupt& )
    {
        KIBITZ_LOG_WARNING( "Caught interrupt" );
    }
    catch( const std::exception& ex )
    {
        KIBITZ_LOG_ERROR( "Exception terminated thread " << ex.what() );
    }

    close_socket( socket );
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz
