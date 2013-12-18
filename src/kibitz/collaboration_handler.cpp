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
#include <kibitz/collaboration_handler.hpp>
#include <kibitz/messages/basic_collaboration_message.hpp>
#include <boost/foreach.hpp>

namespace kibitz
{

////////////////////////////////////////////////////////////////////////////////
collaboration_handler::collaboration_handler(
    context* ctx,
    collaboration_message_bundle_ptr_t message )
    :
    context_( ctx ),
    collaboration_bundle_( message )
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
collaboration_handler::~collaboration_handler()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
void collaboration_handler::operator()()
{
    VLOG( 1 ) << "Entered collaboration handler thread";
    collaboration_callback cb = context_->get_inedge_message_handler();
    if( cb )
    {
        VLOG( 1 ) << "Invoking collaboration callback" ;
        try
        {
            collaboration_messages_t payloads;
            string job_id ;

            BOOST_FOREACH( collaboration_message_ptr_t message,
                           collaboration_bundle_->messages() )
            {
                VLOG( 1 ) << "GOT COLLAB MESSAGES";
                basic_collaboration_message_ptr_t payload_message =
                    static_pointer_cast< basic_collaboration_message >(
                        message );
                job_id = payload_message->job_id();
                payloads.push_back( payload_message->payload() );
            }
            LOG( INFO )
                    << "Invoking message handler for worker type ["
                    << context_->worker_type() << "], Job [" << job_id << "]";
            context_->set_job_id( job_id ) ;
            cb( payloads );
        }
        catch( ... )
        {
            LOG( ERROR ) << "Collaboration callback threw an exception";
        }
    }
    else
    {
        LOG( ERROR )
                << "Recieved a message from another worker that "
                << "there is not a callback defined to handle.";
    }
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz
