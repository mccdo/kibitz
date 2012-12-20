include_directories(${CMAKE_SOURCE_DIR}/src)

#file( GLOB hdrs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.hpp" )
#set(VES_PUBLIC_HEADERS ${hdrs})

set(VES_TARGET_VERSION ${kibitz_VERSION})
set(VES_TARGET_EXPORT ${CMAKE_PROJECT_NAME}-targets)
set(VES_TARGET_CATEGORY Lib)
set(VES_INCLUDE_DIRECTORY_NAME kibitz/locator)

#
#Build the locator library
#
set(VES_PUBLIC_HEADERS
  heartbeat_generator.hpp
  binding_broadcaster.hpp
  router.hpp
  common.hpp
)
set(VES_PRIVATE_HEADERS ${VES_PUBLIC_HEADERS})

add_definitions(-DKIBITZ_LOCATOR_LIBRARY)

add_library( locator SHARED
  heartbeat_generator.cpp
  binding_broadcaster.cpp
  router.cpp
  common.cpp
  ${VES_PUBLIC_HEADERS}
)

target_link_libraries( locator
  kibitz_messages
  kibitz
  validator
  ${Boost_LIBRARIES}
  ${GLOG_LIBRARIES}
  ${ZMQ_LIBRARIES}
)

set(VES_TARGET_NAME locator)
include(VESModuleInstall)
