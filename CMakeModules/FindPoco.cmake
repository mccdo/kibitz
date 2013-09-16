# - Try to find ZMQ
# Once done this will define
#
#  ZMQ_FOUND - system has POCO
#  POCO_INCLUDE_DIRS - the ZMQ include directory
#  POCO_LIBRARIES - Link these to use POCO
#  POCO_DEFINITIONS - Compiler switches required for using POCO
#
#  Copyright (c) 2011 Lee Hambley <lee.hambley@gmail.com>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if (POCO_LIBRARIES AND POCO_INCLUDE_DIRS)
  # in cache already
  set(POCO_FOUND TRUE)
else (POCO_LIBRARIES AND POCO_INCLUDE_DIRS)

  find_path(POCO_INCLUDE_DIR
    NAMES
      Poco.h
    PATHS
      /usr/include/Poco
      /usr/local/include/Poco
      /opt/local/include/Poco
      /sw/include/Poco
      ${POCO_ROOT}/include/Poco
      ENV{POCO_ROOT}/include/Poco
  )

  find_library(POCO_LIBRARY
    NAMES
      PocoFoundation libPocoFoundation
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
      ${POCO_ROOT}/lib
      ENV{POCO_ROOT}/lib
  )

  set(POCO_INCLUDE_DIRS
    ${POCO_INCLUDE_DIR}
  )

  if (POCO_LIBRARY)
    set(POCO_LIBRARIES
        ${POCO_LIBRARIES}
        ${POCO_LIBRARY}
    )
  endif (POCO_LIBRARY)

  find_library(POCO_JSON_LIBRARY 
    NAMES 
      PocoJSON libPocoJSON
    PATHS
      /usr/include/Poco
      /usr/local/include/Poco
      /opt/local/include/Poco
      /sw/include/Poco
      ${POCO_ROOT}/include/Poco
      ENV{POCO_ROOT}/include/Poco
  )

  if( POCO_JSON_LIBRARY )
    set(  POCO_LIBRARIES ${POCO_LIBRARIES} ${POCO_JSON_LIBRARY} )
  endif (POCO_JSON_LIBRARY) 
 

  # show the POCO_INCLUDE_DIRS and POCO_LIBRARIES variables only in the advanced view
  mark_as_advanced(FORCE POCO_INCLUDE_DIRS POCO_LIBRARIES)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(POCO DEFAULT_MSG POCO_LIBRARIES POCO_INCLUDE_DIRS)

endif (POCO_LIBRARIES AND POCO_INCLUDE_DIRS)
