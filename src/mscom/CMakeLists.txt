cmake_minimum_required(VERSION 3.4)
project(fsboxmscom)

include_directories(..)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
aux_source_directory(. SOURCE_FILES)
add_library(fsboxmscom SHARED ${SOURCE_FILES})
target_link_libraries(fsboxmscom libfsbox)

