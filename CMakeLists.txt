cmake_minimum_required(VERSION 3.16)

project(TradingSystems)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#set(CMAKE_BUILD_TYPE Debug)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                TLS_VERIFY ON)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
set(Boost_DEBUG ON)

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_configure(REQUIRES
boost/1.77.0
poco/1.12.4
GENERATORS cmake)
conan_cmake_run(CONANFILE build/conanfile.txt
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

conan_cmake_autodetect(settings)
conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD all
                    REMOTE conancenter
                    SETTINGS
                      ${settings}
                    ) # this method runs conan install it is a wrapper
#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup(TARGETS)

add_executable(main 
    Main.cpp
    )
# must go first to bellow has an existing traget
add_subdirectory(Brokers)
add_subdirectory(Streams)

add_compile_options(-Wall -Wextra -pedantic -pthread) # -pthread

target_include_directories (main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(main 
  CONAN_PKG::boost
  CONAN_PKG::poco )