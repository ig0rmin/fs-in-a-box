
aux_source_directory(. SOURCE_FILES)

if (${WIN32})
	set(SOURCE_FILES ${SOURCE_FILES} "$ENV{BOOST_ROOT}/libs/iostreams/src/mapped_file.cpp")
endif()

add_library(libfsbox STATIC ${SOURCE_FILES})

target_compile_definitions(libfsbox PUBLIC ENABLE_LOGGING)

if (${WIN32})
	target_compile_definitions(libfsbox PUBLIC BOOST_IOSTREAMS_NO_LIB)
	target_compile_definitions(libfsbox PUBLIC _SCL_SECURE_NO_WARNINGS)
else()
	target_link_libraries(libfsbox ${Boost_LIBRARIES})
endif()
