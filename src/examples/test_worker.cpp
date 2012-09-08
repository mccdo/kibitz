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
#include <kibitz/kibitz.hpp>

using std::string;

void message_handler( const kibitz::collaboration_messages_t& messages ) ; 
void notification_handler( );

int main( int argc, char* argv[] ) {
    std::cout << "starting test worker" << std::endl;
    int result = 0;

    try 
    {
        kibitz::initialize( argc, argv );
        kibitz::set_in_message_handler( message_handler );
        kibitz::set_initialization_notification_handler( notification_handler );
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


void message_handler( const kibitz::collaboration_messages_t& messages )  {
  string payload ;
  BOOST_FOREACH( const string& message, messages ) {
    if( payload.empty() ) {
      payload = message;
    } else {
      payload += " > ";
      payload += message;
    }
    
  }

  payload += " > ";
  payload += "next message";

  kibitz::send_out_message( payload );
} 


void notification_handler( ) {
  kibitz::send_out_message( "initial message" );
}
