include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_library(jsoncpp STATIC
json_reader.cpp
json_value.cpp
json_writer.cpp
)
    

