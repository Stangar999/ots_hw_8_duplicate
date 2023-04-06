cmake_minimum_required(VERSION 3.5)

project(tests_p)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (BOOST_ROOT "C:/Program_/Boost/boost_1.81.0_new_MGW")
set(Boost_COMPILER mgw8)

find_package(Boost COMPONENTS program_options REQUIRED)
find_package(Threads REQUIRED)

add_executable(tests_e tests.cpp)
target_include_directories(tests_e PRIVATE ${CMAKE_HOME_DIRECTORY}/src)

add_subdirectory("${CMAKE_HOME_DIRECTORY}/lib/gtest/" googletest)
target_link_libraries(tests_e PRIVATE gtest_main)

target_include_directories(tests_e PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(tests_e PRIVATE ${Boost_LIBRARIES})
