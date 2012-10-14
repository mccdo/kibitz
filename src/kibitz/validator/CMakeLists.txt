include_directories(${CMAKE_SOURCE_DIR}/src)

#file( GLOB hdrs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.hpp" )
#set(VES_PUBLIC_HEADERS ${hdrs})

set(VES_TARGET_VERSION ${kibitz_VERSION})
set(VES_TARGET_EXPORT ${CMAKE_PROJECT_NAME}-targets)
set(VES_TARGET_CATEGORY Lib)
set(VES_INCLUDE_DIRECTORY_NAME kibitz/validator)

#
#Build the validator library
#
set(VES_PUBLIC_HEADERS
  worker_graph.hpp
)
set(VES_PRIVATE_HEADERS ${VES_PUBLIC_HEADERS})

add_definitions(-DKIBITZ_VALIDATOR_LIBRARY)

add_library( validator SHARED
  worker_graph.cpp
  ${VES_PUBLIC_HEADERS}
)

target_link_libraries( validator
  ${Boost_LIBRARIES}
  ${GLOG_LIBRARIES}
)

set(VES_TARGET_NAME validator)
include(VESModuleInstall)
