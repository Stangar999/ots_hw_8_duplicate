cmake_minimum_required(VERSION 3.5)

project(bayan_p LANGUAGES CXX)

## путь к коревому каталогу буста (например: с:/boost)
set (BOOST_ROOT "C:/Program_/Boost/boost_1.81.0_new_MGW")
set(Boost_COMPILER mgw8)

find_package(Boost COMPONENTS program_options regex REQUIRED)
find_package(Threads REQUIRED)

add_executable(bayan_e main.cpp
  serch_params.h
  common.h
  duplicate_search.cpp
  duplicate_search.h
)

target_include_directories(bayan_e PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(bayan_e ${Boost_LIBRARIES})
