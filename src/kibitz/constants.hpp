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
#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__

namespace kibitz
{
static const char* SHUTDOWN_MSG = "shutdown";

static const char* INPROC_COMMAND_BINDING = "inproc://msgbus";

  /// Inproc binding used by socket that sends message to publisher which will
  /// be forwarded to locator
  static const char* INPROC_LOCATOR_PUBLISH_BINDING="inproc://locatorpubbinding";

  /// Inproc binding used by socket that sends notification messages
  static const char* INPROC_NOTIFICATION_PUBLISH_BINDING="inproc://notificationpubbinding";
}

#endif

