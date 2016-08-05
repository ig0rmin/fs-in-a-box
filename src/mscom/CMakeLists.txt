
aux_source_directory(. SOURCE_FILES)

add_library(fsboxmscom SHARED ${SOURCE_FILES})

target_link_libraries(fsboxmscom libfsbox)
