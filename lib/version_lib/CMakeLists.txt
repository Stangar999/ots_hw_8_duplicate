cmake_minimum_required(VERSION 3.5)

#project(version_p LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#configure_file(version_define.h.in version_define.h)

add_library(version_l version_define.h.in version.cpp)

target_include_directories(version_l
    PRIVATE "${CMAKE_BINARY_DIR}"
)



