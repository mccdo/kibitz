include_directories(${CMAKE_SOURCE_DIR}/src)

set(VES_TARGET_VERSION ${kibitz_VERSION})
set(VES_TARGET_EXPORT ${CMAKE_PROJECT_NAME}-targets)
set(VES_INCLUDE_DIRECTORY_NAME ${CMAKE_PROJECT_NAME})

#
#Build the executables
#
set(VES_TARGET_CATEGORY App)

add_executable( worker-locator locator.cpp ) 
target_link_libraries( worker-locator locator kibitz )
set(VES_TARGET_NAME worker-locator)
include(VESModuleInstall)
