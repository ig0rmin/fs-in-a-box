cmake_minimum_required(VERSION 3.4)
project(libfsbox)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
aux_source_directory(. SOURCE_FILES)

add_library(libfsbox STATIC ${SOURCE_FILES})