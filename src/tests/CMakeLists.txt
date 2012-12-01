include_directories(${CMAKE_SOURCE_DIR}/src)

set(VES_TARGET_VERSION ${kibitz_VERSION})
set(VES_TARGET_EXPORT ${CMAKE_PROJECT_NAME}-targets)
set(VES_INCLUDE_DIRECTORY_NAME ${CMAKE_PROJECT_NAME})

#
#Build the executables
#
set(VES_TARGET_CATEGORY Test)

add_executable( unit-test unit_test.cpp )
target_link_libraries(unit-test kibitz ${Boost_LIBRARIES})
set(VES_TARGET_NAME unit-test)
include(VESModuleInstall)

add_executable( locator-test locator_test.cpp ) 
target_link_libraries( locator-test locator kibitz ${Boost_LIBRARIES} )
set(VES_TARGET_NAME locator-test)
include(VESModuleInstall)

add_executable( regression-tester regression_tester.cpp )
target_link_libraries( regression-tester kibitz ${Boost_LIBRARIES} )
set(VES_TARGET_NAME regression-tester)
include(VESModuleInstall)
