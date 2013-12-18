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
#include <kibitz/initialization_handler.hpp>

namespace kibitz
{

////////////////////////////////////////////////////////////////////////////////
initialization_handler::initialization_handler(
    context* ctx,
    const string& initialization_data )
    :
    context_( ctx ),
    initialization_data_( initialization_data ),
    m_logger( Poco::Logger::get("initialization_handler") )
{
    m_logStream = LogStreamPtr( new Poco::LogStream( m_logger ) );
}
////////////////////////////////////////////////////////////////////////////////
initialization_handler::~initialization_handler()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
void initialization_handler::operator()()
{
    KIBITZ_LOG_DEBUG( "Entered initialization handler thread" );
    initialization_callback cb =
        context_->get_initialization_notification_callback();
    if( cb )
    {
        KIBITZ_LOG_DEBUG( "Invoking initialization callback" );
        try
        {
            cb( initialization_data_ );
        }
        catch( ... )
        {
            KIBITZ_LOG_ERROR( "Job initialization callback threw an exception" );
        }
    }
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz
