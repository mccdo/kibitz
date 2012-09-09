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

#if defined(_MSC_VER)
#pragma warning( disable : 4503 )
#pragma warning( disable : 4251 )
#endif

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  if defined( KIBITZ_LIBRARY_STATIC )
#    define KIBITZ_EXPORT
#    define KIBITZ_LOCAL
#  elif defined( KIBITZ_LIBRARY )
#    define KIBITZ_EXPORT   __declspec(dllexport)
#    define KIBITZ_LOCAL
#  else
#    define KIBITZ_EXPORT   __declspec(dllimport)
#    define KIBITZ_LOCAL
#  endif

#  if defined( KIBITZ_MESSAGE_LIBRARY_STATIC )
#    define KIBITZ_MESSAGE_EXPORT
#    define KIBITZ_MESSAGE_LOCAL
#  elif defined( KIBITZ_MESSAGE_LIBRARY )
#    define KIBITZ_MESSAGE_EXPORT   __declspec(dllexport)
#    define KIBITZ_MESSAGE_LOCAL
#  else
#    define KIBITZ_MESSAGE_EXPORT   __declspec(dllimport)
#    define KIBITZ_MESSAGE_LOCAL
#  endif

#  if defined( KIBITZ_LOCATOR_LIBRARY_STATIC )
#    define KIBITZ_LOCATOR_EXPORT
#    define KIBITZ_LOCATOR_LOCAL
#  elif defined( KIBITZ_LOCATOR_LIBRARY )
#    define KIBITZ_LOCATOR_EXPORT   __declspec(dllexport)
#    define KIBITZ_LOCATOR_LOCAL
#  else
#    define KIBITZ_LOCATOR_EXPORT   __declspec(dllimport)
#    define KIBITZ_LOCATOR_LOCAL
#  endif
#else
#if __GNUC__ >= 4
# if defined( KIBITZ_LIBRARY_STATIC )
#    define KIBITZ_EXPORT
#    define KIBITZ_LOCAL
# else
#    define KIBITZ_EXPORT   __attribute__ ((visibility ("default")))
#    define KIBITZ_LOCAL   __attribute__ ((visibility ("hidden")))
# endif

# if defined( KIBITZ_MESSAGE_LIBRARY_STATIC )
#    define KIBITZ_MESSAGE_EXPORT
#    define KIBITZ_MESSAGE_LOCAL
# else
#    define KIBITZ_MESSAGE_EXPORT   __attribute__ ((visibility ("default")))
#    define KIBITZ_MESSAGE_LOCAL   __attribute__ ((visibility ("hidden")))
# endif

# if defined( KIBITZ_LOCATOR_LIBRARY_STATIC )
#    define KIBITZ_LOCATOR_EXPORT
#    define KIBITZ_LOCATOR_LOCAL
# else
#    define KIBITZ_LOCATOR_EXPORT   __attribute__ ((visibility ("default")))
#    define KIBITZ_LOCATOR_LOCAL   __attribute__ ((visibility ("hidden")))
# endif
#else
#  define KIBITZ_MESSAGE_EXPORT
#  define KIBITZ_MESSAGE_LOCAL

#  define KIBITZ_EXPORT
#  define KIBITZ_LOCAL

#  define KIBITZ_LOCATOR_EXPORT
#  define KIBITZ_LOCATOR_LOCAL
#endif
#endif


#if defined(_MSC_VER)
#define KIBITZ_DEPRECATED(x) __declspec(deprecated) x
#else
#define KIBITZ_DEPRECATED(x) x __attribute__ ((deprecated))
#endif
