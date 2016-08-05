
include_directories(.) # solves problem with include "gtest/gest.h" from "gmock/gmock.h"

aux_source_directory(. SOURCE_FILES)

add_executable(fsboxunit ${SOURCE_FILES})

target_link_libraries(fsboxunit libfsbox)

if (${UNIX})
	target_link_libraries(fsboxunit "pthread")
endif()