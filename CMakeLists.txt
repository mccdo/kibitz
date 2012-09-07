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
find_package(Boost 1.44.0 REQUIRED COMPONENTS prg_exec_monitor program_options regex chrono date_time filesystem system thread )

set( GLOG_ROOT "Please specify" CACHE PATH "Root directory where glog is installed" )
find_package(glog REQUIRED)

set( ZMQ_ROOT "Please specify" CACHE PATH "Root directory where ZMQ is installed" )
find_package(ZMQ REQUIRED)

set( YAMLCPP_ROOT "Please specify" CACHE PATH "Root directory where yaml-cpp is installed" )
find_package(yamlcpp REQUIRED)

INCLUDE_DIRECTORIES( 
    ${Boost_INCLUDE_DIRS}
    ${GLOG_INCLUDE_DIRS}
    ${ZMQ_INCLUDE_DIRS}
    ${YAMLCPP_INCLUDE_DIRS}
)

if(WIN32)
    add_definitions( -DBOOST_ALL_DYN_LINK )
else(WIN32)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo")
    #set( CMAKE_CXX_FLAGS "-g" )
endif(WIN32)

include(VESSetupCompiler)

include(VESBinAndLibDirs)

add_subdirectory(src)
add_subdirectory(script)

include(VESInstallPDBFiles)

set(VES_EXPORT_LIBRARY_TARGETS kibitz kibitz_messages locator)
set(VES_CONFIGURE_VARS CMAKE_INSTALL_LIBDIR)

if(WIN32)
    set(${CMAKE_PROJECT_NAME}_FPC_LIBRARIES "/libpath:\"\${libdir}\" kibitz_messages.lib kibitz.lib locator.lib")
    set(${CMAKE_PROJECT_NAME}_FPC_INCLUDES "/I\"\${includedir}\"")
else(WIN32)
    set(${CMAKE_PROJECT_NAME}_FPC_INCLUDES "-I\${includedir}")
    set(${CMAKE_PROJECT_NAME}_FPC_LIBRARIES "-L\${libdir} -lkibitz_messages -lkibitz -llocator")
endif(WIN32)

include(VESInstallAutoDiscoveryTools)
