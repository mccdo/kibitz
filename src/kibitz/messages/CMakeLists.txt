include_directories(${CMAKE_SOURCE_DIR}/src)

#file( GLOB hdrs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.hpp" )
#set(VES_PUBLIC_HEADERS ${hdrs})

set(VES_TARGET_VERSION ${kibitz_VERSION})
set(VES_TARGET_EXPORT ${CMAKE_PROJECT_NAME}-targets)
set(VES_TARGET_CATEGORY Lib)
set(VES_INCLUDE_DIRECTORY_NAME kibitz/messages)

#add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})

#
#Build the kibitz_messages library
#
set(VES_PUBLIC_HEADERS
    heartbeat.hpp
    message.hpp
    inproc_notification_message.hpp
    worker_notification_message.hpp
    worker_broadcast_message.hpp
    worker_notification.hpp
    job_initialization_message.hpp
    basic_collaboration_message.hpp
    collaboration_message.hpp
    collaboration_message_bundle.hpp
    notification_message.hpp
    binding_notification.hpp
    worker_status_message.hpp
)
set(VES_PRIVATE_HEADERS ${VES_PUBLIC_HEADERS})

add_definitions(-DKIBITZ_MESSAGE_LIBRARY)

add_library( kibitz_messages SHARED 
  heartbeat.cpp
  message.cpp
  inproc_notification_message.cpp
  worker_notification_message.cpp
  worker_broadcast_message.cpp
  job_initialization_message.cpp
  basic_collaboration_message.cpp
  binding_notification.cpp
  collaboration_message_bundle.cpp
  worker_notification.cpp
  worker_status_message.cpp
  ${VES_PUBLIC_HEADERS}
)

target_link_libraries( kibitz_messages 
  ${Boost_LIBRARIES}
  ${GLOG_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
)

set(VES_TARGET_NAME kibitz_messages)
include(VESModuleInstall)
