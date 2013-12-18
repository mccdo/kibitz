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
#include <stdexcept>
#include <string>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <glog/logging.h>
#include <kibitz/kibitz.hpp>

using std::string;

typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

void message_handler( const kibitz::collaboration_messages_t& messages ) ;


int main( int argc, char* argv[] )
{
    std::cout << "starting test worker" << std::endl;
    int result = 0;

    try
    {
        kibitz::initialize( argc, argv );
        // set the callback that will be invoked when
        // collaboration messages arrive
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

/////////////////////////////////////////////////////////////////////
//  For workers in a collaboration we implement a message
//  handler.  You must define the in edges in a yaml file of the
//  form
//  ---
//  in-edges:
//  - edge1
//  - edge2
//  Where edge1 and edge2 are the worker types that we expect messages
//  from.  The yaml file is passed on the command line.  The
//  messages arguement will contain a message from each edge.
//  The callback is not invoked until all expected messages for
//  a particular job have arrived.  When the logic in the
//  callback is complete, call send_out_message to send the
//  results to downstream workers in the collaboration.
/////////////////////////////////////////////////////////////////////
void message_handler( const kibitz::collaboration_messages_t& messages )
{

    DLOG( INFO ) << "Got messages" ;
    CHECK( messages.size() == 1 ) << "Expect a single inbound message" ;

    string role = getenv( "ROLE" );

    CHECK( !role.empty() ) << "ROLE environment variable not set?";

    boost::char_separator<char> sep( ";" );
    tokenizer tokens( messages.front(), sep );
    int result = role == "adder" ? 0 : 1;
    BOOST_FOREACH( const string & number, tokens )
    {
        if( role == "adder" )
        {
            result += boost::lexical_cast<int>( number );
        }
        if( role == "multiplier" )
        {
            result *= boost::lexical_cast<int>( number );
        }
    }

    DLOG( INFO ) << "Sending out message -> " << result;
    kibitz::send_out_message( boost::lexical_cast<string>( result ) );
}



