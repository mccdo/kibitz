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
#ifndef worker_info_hpp
#define worker_info_hpp

#include <kibitz/messages/notification_message.hpp>


namespace kibitz {
  struct worker_info {
    string worker_type;
    string host;
    int worker_id ;
    int port;
    bool operator==( const worker_info& rval ) {
      if( worker_type == rval.worker_type ) 
	if( host == rval.host )
	  if( worker_id == rval.worker_id )
	    if( port == rval.port )
	      return true;

      return false;

    }
    bool operator!=( const worker_info& rval ) {
      return !operator==( rval );
    }
  };

  typedef std::vector< worker_info > worker_infos_t;



}





#endif
