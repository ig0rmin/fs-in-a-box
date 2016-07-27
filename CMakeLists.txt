cmake_minimum_required(VERSION 3.4)
project(fs-in-a-box)

add_subdirectory(src/libfsbox)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_subdirectory(src/mscom)
else()
	add_subdirectory(src/xpcom)
endif()

