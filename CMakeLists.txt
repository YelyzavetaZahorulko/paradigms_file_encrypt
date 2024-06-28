cmake_minimum_required(VERSION 3.28)
project(paradigms_file_encrypt)

set(CMAKE_CXX_STANDARD 17)

add_executable(paradigms_file_encrypt main.cpp
        caesar.cpp
        caesar.h)

add_library(caesar SHARED caesar.cpp)

add_executable(main main.cpp)

target_link_libraries(main caesar)

set_target_properties(caesar PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
