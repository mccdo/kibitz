include_directories(${CMAKE_SOURCE_DIR}/src)

#file( GLOB hdrs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.hpp" )
#set(VES_PUBLIC_HEADERS ${hdrs})

set(VES_TARGET_VERSION ${kibitz_VERSION})
set(VES_TARGET_EXPORT ${CMAKE_PROJECT_NAME}-targets)
set(VES_TARGET_CATEGORY Lib)
set(VES_INCLUDE_DIRECTORY_NAME ${CMAKE_PROJECT_NAME})

#add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})

#
#Build the kibitz_messages library
#
set(VES_PUBLIC_HEADERS
    heartbeat.hpp
    message.hpp
    inproc_notification_message.hpp
    worker_notification_message.hpp
    worker_info.hpp
    worker_query.hpp
    worker_query_response.hpp
    worker_broadcast_message.hpp
    job_initialization_message.hpp
    basic_collaboration_message.hpp
    collaboration_message.hpp
    notification_message.hpp
)
set(VES_PRIVATE_HEADERS ${VES_PUBLIC_HEADERS})

add_definitions(-DKIBITZ_MESSAGE_LIBRARY)

add_library( kibitz_messages SHARED 
  heartbeat.cpp
  message.cpp
  inproc_notification_message.cpp
  worker_notification_message.cpp
  worker_query.cpp
  worker_query_response.cpp
  worker_broadcast_message.cpp
  job_initialization_message.cpp
  basic_collaboration_message.cpp
  ${VES_PUBLIC_HEADERS}
)

target_link_libraries( kibitz_messages 
  ${Boost_LIBRARIES}
)

set(VES_TARGET_NAME kibitz_messages)
include(VESModuleInstall)

string(REPLACE "/DKIBITZ_MESSAGE_LIBRARY" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

#
#Build the kibitz library
#
set(VES_PUBLIC_HEADERS
    kibitz.hpp
    heartbeat_sender.hpp
    context.hpp
    message_base.hpp
    common.hpp
    bindings.hpp
    heartbeat_receiver.hpp
    kibitz_util.hpp
    worker_map.hpp
    bus.hpp
    in_edge_manager.hpp
    basic_collaboration_message.hpp
)
set(VES_PRIVATE_HEADERS ${VES_PUBLIC_HEADERS})

add_definitions(-DKIBITZ_LIBRARY)

add_library( kibitz SHARED
  kibitz.cpp
  heartbeat_sender.cpp
  context.cpp
  message_base.cpp
  common.cpp
  heartbeat_receiver.cpp
  kibitz_util.cpp
  worker_map.cpp
  bus.cpp
  in_edge_manager.cpp
  basic_collaboration_message.cpp
  ${VES_PUBLIC_HEADERS}
)

target_link_libraries( kibitz
  kibitz_messages		       
  ${Boost_LIBRARIES}
  ${GLOG_LIBRARIES}
  ${ZMQ_LIBRARIES}
  ${YAMLCPP_LIBRARIES}
)

set(VES_TARGET_NAME kibitz)
include(VESModuleInstall)

string(REPLACE "/DKIBITZ_LIBRARY" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

#
#Build the locator library
#
set(VES_PUBLIC_HEADERS
  registry.hpp
)
set(VES_PRIVATE_HEADERS ${VES_PUBLIC_HEADERS})

add_definitions(-DKIBITZ_LOCATOR_LIBRARY)

add_library( locator SHARED
  registry.cpp
  ${VES_PUBLIC_HEADERS}
)

target_link_libraries( locator
  kibitz_messages
  kibitz
  ${Boost_LIBRARIES}
  ${GLOG_LIBRARIES}
  ${ZMQ_LIBRARIES}
)

set(VES_TARGET_NAME locator)
include(VESModuleInstall)

string(REPLACE "/DKIBITZ_LOCATOR_LIBRARY" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

#
#Build the executables
#
set(VES_TARGET_CATEGORY Test)

add_executable( test-worker  test_worker.cpp )
target_link_libraries( test-worker kibitz )
set(VES_TARGET_NAME test-worker)
include(VESModuleInstall)

add_executable( unit-test unit_test.cpp )
target_link_libraries(unit-test kibitz ${Boost_LIBRARIES})
set(VES_TARGET_NAME unit-test)
include(VESModuleInstall)

add_executable( worker-locator locator.cpp ) 
target_link_libraries( worker-locator locator kibitz)
set(VES_TARGET_NAME worker-locator)
include(VESModuleInstall)

add_executable( regression-tester regression_tester.cpp )
target_link_libraries( regression-tester kibitz )
set(VES_TARGET_NAME regression-tester)
include(VESModuleInstall)
