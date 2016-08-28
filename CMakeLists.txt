cmake_minimum_required(VERSION 2.4)
project(fs-in-a-box)

cmake_policy(SET CMP0003 NEW)

if (${WIN32})
	include_directories($ENV{BOOST_ROOT})
else()
	find_package(Boost COMPONENTS iostreams REQUIRED)
	include_directories(${Boost_INCLUDE_DIR})
endif()

include_directories(src)

#I dunno why set_property(GLOBAL PROPERTY CXX_STANDARD 11) is not working with GCC
if (${CMAKE_COMPILER_IS_GNUCC})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
	set_property(GLOBAL PROPERTY CXX_STANDARD 11)
endif()

add_subdirectory(src/libfsbox)
add_subdirectory(test)

if (${WIN32})
	add_subdirectory(src/mscom)
else()
	add_subdirectory(src/xpcom)
endif()

