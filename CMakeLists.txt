cmake_minimum_required(VERSION 2.4)
project(fs-in-a-box)

include_directories($ENV{BOOST_ROOT})
include_directories(src)
set_property(GLOBAL PROPERTY CXX_STANDARD 11)

add_subdirectory(src/libfsbox)
add_subdirectory(test)

if (${WIN32})
	add_subdirectory(src/mscom)
else()
	add_subdirectory(src/xpcom)
endif()

