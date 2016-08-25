
aux_source_directory(. SOURCE_FILES)

set(SOURCE_FILES ${SOURCE_FILES} fsboxmscom.def)

add_library(fsboxmscom SHARED ${SOURCE_FILES})

target_link_libraries(fsboxmscom libfsbox)