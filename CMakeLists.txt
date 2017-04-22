cmake_minimum_required(VERSION 2.8.7)

set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
find_package(OpenSSL QUIET)

include_directories(include)
file(GLOB LIB_SOURCES "src/*.c")

project (progbase)

if(NOT OPENSSL_FOUND)
    message(WARNING "SSL library no found. Install libssl-dev package.")
    add_definitions(-DSSL_MISSING)
endif()

add_library(${CMAKE_PROJECT_NAME} STATIC ${LIB_SOURCES})
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY C_STANDARD 99)

install(
    DIRECTORY include/ DESTINATION /usr/local/include
    FILES_MATCHING PATTERN "*.h"
)
install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION /usr/local/lib)

SET(CMAKE_C_FLAGS "-std=c99 -Wall")

