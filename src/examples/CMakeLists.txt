include_directories(${CMAKE_SOURCE_DIR}/src)

set(VES_TARGET_VERSION ${kibitz_VERSION})
set(VES_TARGET_EXPORT ${CMAKE_PROJECT_NAME}-targets)
set(VES_INCLUDE_DIRECTORY_NAME ${CMAKE_PROJECT_NAME})
#
#Build the executables
#
set(VES_TARGET_CATEGORY Test)

add_executable( worker  worker.cpp )
add_executable( init-worker init_worker.cpp )
add_executable( terminus terminus.cpp )
target_link_libraries( worker kibitz )
target_link_libraries( init-worker kibitz )
target_link_libraries( terminus kibitz )
set(VES_TARGET_NAME worker)
include(VESModuleInstall)


