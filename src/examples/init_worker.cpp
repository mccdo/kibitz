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
#include <fstream>
#include <stdexcept>
#include <string>
#include <boost/foreach.hpp>
#include <glog/logging.h>
#include <kibitz/kibitz.hpp>

using std::string;


void notification_handler( );

int main( int argc, char* argv[] )
{
  std::cout << "starting test worker" << std::endl;
  int result = 0;

  try
    {
      kibitz::initialize( argc, argv );
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


//////////////////////////////////////////////////////////
//  Get things started, this notification is triggered
//  to start the job.  Initialization of job might include
//  fetching start up data from a database, or a web service.
//  Another source of data might be the notification 
//  sent when a different job is completed.  In this 
//  case we read several lines of numbers from a file
//  cocantenate them with each separated by a semicolon
//  send then send the result to listening workers. 
/////////////////////////////////////////////////////////
void notification_handler( ) {
  string filename = getenv("INITDATA");
  DLOG(INFO) << "Reading initial data from file " <<  filename;
  std::ifstream stm( filename.c_str() );
  string result;
  while( stm.good() ) {
    if( !result.empty() ) {
      result += ";";
    }
    string line;
    std::getline( stm, line );
    result += line;
  }

  DLOG(INFO) << "Sending out message -> " << result;
  kibitz::send_out_message( result );
}
