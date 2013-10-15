cmake_minimum_required ( VERSION 2.8.8 )

project (kibitz) 

set( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules" )

set(${CMAKE_PROJECT_NAME}_MAJOR_VERSION 0)
set(${CMAKE_PROJECT_NAME}_MINOR_VERSION 1)
set(${CMAKE_PROJECT_NAME}_SUB_VERSION 0)
set(${CMAKE_PROJECT_NAME}_VERSION ${${CMAKE_PROJECT_NAME}_MAJOR_VERSION}.${${CMAKE_PROJECT_NAME}_MINOR_VERSION}.${${CMAKE_PROJECT_NAME}_SUB_VERSION})
set(VES_PACKAGE_VERSION ${${CMAKE_PROJECT_NAME}_VERSION})

set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)


set( BOOST_ROOT "Please specify" CACHE PATH "Root directory where Boost is installed" )

if(WIN32)
	find_package(Boost 1.53.0 REQUIRED COMPONENTS program_options regex chrono date_time filesystem system thread prg_exec_monitor )
else(WIN32)
	find_package(Boost 1.53.0 REQUIRED COMPONENTS program_options regex chrono date_time filesystem system thread )
endif(WIN32)

set( GLOG_ROOT "Please specify" CACHE PATH "Root directory where glog is installed" )
find_package(glog REQUIRED)

set( ZMQ_ROOT "Please specify" CACHE PATH "Root directory where ZMQ is installed" )
find_package(ZMQ REQUIRED)

set( YAMLCPP_ROOT "Please specify" CACHE PATH "Root directory where yaml-cpp is installed" )
find_package(yamlcpp REQUIRED)

set( POCO_ROOT "Please specify" CACHE PATH "Root directory where Poco is installed" )
find_package( POCO REQUIRED COMPONENTS Foundation JSON )

find_package(Threads)

INCLUDE_DIRECTORIES( 
    ${Boost_INCLUDE_DIRS}
    ${GLOG_INCLUDE_DIRS}
    ${ZMQ_INCLUDE_DIRS}
    ${YAMLCPP_INCLUDE_DIRS}
    ${POCO_INCLUDE_DIR}
)

if(WIN32)
    add_definitions( -DBOOST_ALL_DYN_LINK )
    add_definitions( -D_WIN32_WINNT=0x0501 )
else(WIN32)
    set(CMAKE_BUILD_TYPE "Release")
    #set( CMAKE_CXX_FLAGS "-g -O0" )
endif(WIN32)

include(VESSetupCompiler)

include(VESBinAndLibDirs)

add_subdirectory(src)
add_subdirectory(script)

include(VESInstallPDBFiles)

set(VES_EXPORT_LIBRARY_TARGETS kibitz kibitz_messages locator validator)
set(VES_CONFIGURE_VARS CMAKE_INSTALL_LIBDIR)

if(WIN32)
    set(${CMAKE_PROJECT_NAME}_FPC_LIBRARIES "/libpath:\"\${libdir}\" kibitz_messages.lib kibitz.lib locator.lib validator.lib")
    set(${CMAKE_PROJECT_NAME}_FPC_INCLUDES "/I\"\${includedir}\"")
else(WIN32)
    set(${CMAKE_PROJECT_NAME}_FPC_INCLUDES "-I\${includedir}")
    set(${CMAKE_PROJECT_NAME}_FPC_LIBRARIES "-L\${libdir} -lkibitz_messages -lkibitz -llocator -lvalidator")
endif(WIN32)

include(VESInstallAutoDiscoveryTools)


#
# Doxygen
find_package( Doxygen )
if( DOXYGEN_FOUND )
    set( HAVE_DOT "NO" )
    if( DOXYGEN_DOT_PATH )
        set( HAVE_DOT "YES" )
    endif()
    
    configure_file( ${PROJECT_SOURCE_DIR}/Doxyfile
        ${PROJECT_BINARY_DIR}/doc/Doxyfile
    )
    add_custom_target( Documentation ${DOXYGEN_EXECUTABLE}
        ${PROJECT_BINARY_DIR}/doc/Doxyfile
    )
    
    #add_subdirectory( doc/images )
endif()
