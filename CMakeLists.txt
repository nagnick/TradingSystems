cmake_minimum_required(VERSION 3.16)

project(TradingSystems)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#set(CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -Wextra -pedantic -pthread) # -pthread
add_executable(main 
    Main.cpp
    )

target_include_directories (main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})