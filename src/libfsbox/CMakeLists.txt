
aux_source_directory(. SOURCE_FILES)

set(SOURCE_FILES ${SOURCE_FILES} "$ENV{BOOST_ROOT}/libs/iostreams/src/mapped_file.cpp")

add_library(libfsbox STATIC ${SOURCE_FILES})

target_compile_definitions(libfsbox PUBLIC BOOST_IOSTREAMS_NO_LIB)
target_compile_definitions(libfsbox PUBLIC ENABLE_LOGGING)
target_compile_definitions(libfsbox PUBLIC _SCL_SECURE_NO_WARNINGS)
