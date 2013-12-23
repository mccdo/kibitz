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
#include <iostream>
#include <string>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <kibitz/logging.hpp>
#include <kibitz/kibitz.hpp>

typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

void message_handler( const kibitz::collaboration_messages_t& messages ) ;

int main( int argc, char* argv[] )
{
    std::cout << "starting test worker" << std::endl;
    int result = 0;

    try
    {
        kibitz::initialize( argc, argv );
        // set callback that will be invoked when
        // upstream workers send us messages
        kibitz::set_in_message_handler( message_handler );
        kibitz::start();
        kibitz::terminate();
    }
    catch( std::exception& e )
    {
        result = 1;
        std::cout << "Program failed. " << e.what() << std::endl;
    }

    return result;


}

////////////////////////////////////////////////////////////////////////
//  This is a message handler for a terminal worker.  A terminal worker
//  is the last worker called in a collaboration and does not forward
//  messages to other workers, the job is done.
//  A terminal worker calls send_notification_message instead of
//  send_out_message.  send_notification_message will publish results
//  to other services that are listening outside the collaboration.  You
//  could think of this as a signal that the job is complete.  You could
//  just as well write data to a database or a file at this point, the
//  notification call is not required.
////////////////////////////////////////////////////////////////////////
void message_handler( const kibitz::collaboration_messages_t& messages )
{

    KIBITZ_STATIC_LOG_NOTICE( "terminus", "Got messages" );
    poco_assert( messages.size() == 2 );// << "Expect a two messages" ;
    std::string result ;

    BOOST_FOREACH( const std::string& message, messages )
    {
        if( !result.empty() )
        {
            result += ";";
        }
        result += message;
    }

    // Note send notification instead of send out message.
    KIBITZ_STATIC_LOG_NOTICE( "terminus", "Sending notification -> " << result );
    kibitz::send_notification_message( result );
}



