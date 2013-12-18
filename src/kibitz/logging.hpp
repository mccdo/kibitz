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
#pragma once

// New logging tools based on Poco::Logger and Poco::LogStream

#include <Poco/Logger.h>
#include <Poco/LogStream.h>

#include <boost/shared_ptr.hpp>

// Smart pointer-ized version of LogStream to automate memory management
namespace kibitz
{
typedef boost::shared_ptr<Poco::LogStream>  LogStreamPtr;
}

#define _KIBITZ_LOG_CALL( prio, msg ) do {if( m_logger.prio() ){ (*m_logStream).prio() << msg << std::endl; } } while( 0 )
#define _KIBITZ_STATIC_LOG_CALL( prio, name, msg ) do { Poco::Logger& logger = Poco::Logger::get( name ); if( logger.prio() ){Poco::LogStream logstream( logger ); logstream.information() << msg << std::endl; } } while( 0 )

/// Use these logging macros in classes that have a dedicated m_logger and m_logStream.
/// This method has faster execution that the static method below since the log
/// is only requested once and a reference to it is stored in the class's m_logger
/// member.
#define KIBITZ_LOG_ALWAYS( msg ) _KIBITZ_LOG_CALL( fatal, msg )
#define KIBITZ_LOG_FATAL( msg ) _KIBITZ_LOG_CALL( fatal, msg )
#define KIBITZ_LOG_CRITICAL( msg ) _KIBITZ_LOG_CALL( critical, msg )
#define KIBITZ_LOG_ERROR( msg ) _KIBITZ_LOG_CALL( error, msg )
#define KIBITZ_LOG_WARNING( msg ) _KIBITZ_LOG_CALL( warning, msg )
#define KIBITZ_LOG_NOTICE( msg ) _KIBITZ_LOG_CALL( notice, msg )
#define KIBITZ_LOG_INFO( msg ) _KIBITZ_LOG_CALL( information, msg )

/// Use these logging macros for one-off log messages and inside static methods
/// or other places not associated with a class. The name argument is the name
/// of the log to use. The name is in the form parent.child.subchild.subsubchild
/// where everything after parent is optional. However, if requesting parent.child.subchild,
/// a log named parent.child must have been previously requested, otherwise the
/// log call fails silently. Likewise, if requesting parent.child, the parent log
/// must have been previously requested.
#define KIBITZ_STATIC_LOG_ALWAYS( name, msg ) _KIBITZ_STATIC_LOG_CALL( fatal, name, msg )
#define KIBITZ_STATIC_LOG_FATAL( name, msg ) _KIBITZ_STATIC_LOG_CALL( fatal, name, msg )
#define KIBITZ_STATIC_LOG_CRITICAL( name, msg ) _KIBITZ_STATIC_LOG_CALL( critical, name, msg )
#define KIBITZ_STATIC_LOG_ERROR( name, msg ) _KIBITZ_STATIC_LOG_CALL( error, name, msg )
#define KIBITZ_STATIC_LOG_WARNING( name, msg ) _KIBITZ_STATIC_LOG_CALL( warning, name, msg )
#define KIBITZ_STATIC_LOG_NOTICE( name, msg ) _KIBITZ_STATIC_LOG_CALL( notice, name, msg )
#define KIBITZ_STATIC_LOG_INFO( name, msg ) _KIBITZ_STATIC_LOG_CALL( information, name, msg )


// DEBUG and TRACE log messages will only be compiled in when KIBITZ_DEBUG
// is defined. This allows the liberal use of DEBUG and TRACE messages to
// track down difficult bugs without affecting performace of production builds.
// KIBITZ_DEBUG can either be defined via the build system for the entire build or can be
// individually defined in a single class before the include statement for
// this file.
#if defined(KIBITZ_DEBUG)
    #define KIBITZ_LOG_DEBUG( msg ) _KIBITZ_LOG_CALL( debug, msg )
    #define KIBITZ_LOG_TRACE( msg ) _KIBITZ_LOG_CALL( trace, msg )
    #define KIBITZ_STATIC_LOG_DEBUG( name, msg ) _KIBITZ_STATIC_LOG_CALL( debug, name, msg )
    #define KIBITZ_STATIC_LOG_TRACE( name, msg ) _KIBITZ_STATIC_LOG_CALL( trace, name, msg )
#else
    #define KIBITZ_LOG_DEBUG( msg )
    #define KIBITZ_LOG_TRACE( msg )
    #define KIBITZ_STATIC_LOG_DEBUG( name, msg )
    #define KIBITZ_STATIC_LOG_TRACE( name, msg )
#endif
