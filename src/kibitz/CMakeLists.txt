include_directories(${CMAKE_SOURCE_DIR}/src)

#file( GLOB hdrs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.hpp" )
#set(VES_PUBLIC_HEADERS ${hdrs})

set(VES_TARGET_VERSION ${kibitz_VERSION})
set(VES_TARGET_EXPORT ${CMAKE_PROJECT_NAME}-targets)
set(VES_TARGET_CATEGORY Lib)
set(VES_INCLUDE_DIRECTORY_NAME kibitz)

#
#Build the kibitz library
#
set(VES_PUBLIC_HEADERS
    GNUCompilerGuards.hpp
    export_config.hpp
    kibitz.hpp
    context.hpp
    message_base.hpp
    common.hpp
    bindings.hpp
    heartbeat_receiver.hpp
    kibitz_util.hpp
    bus.hpp
    in_edge_manager.hpp
    publisher.hpp
    initialization_handler.hpp
    collaboration_handler.hpp
)
set(VES_PRIVATE_HEADERS ${VES_PUBLIC_HEADERS})

add_definitions(-DKIBITZ_LIBRARY)

add_library( kibitz SHARED
  kibitz.cpp
  context.cpp
  message_base.cpp
  common.cpp
  heartbeat_receiver.cpp
  kibitz_util.cpp
  bus.cpp
  in_edge_manager.cpp
  publisher.cpp
  initialization_handler.cpp
  collaboration_handler.cpp
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

add_subdirectory(messages)
add_subdirectory(locator)
add_subdirectory(validator)
