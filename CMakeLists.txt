cmake_minimum_required(VERSION 2.8.7)
include_directories(include)
file(GLOB LIB_SOURCES "src/*.c")
project (progbase)
add_library(${CMAKE_PROJECT_NAME} STATIC ${LIB_SOURCES})

install(
    DIRECTORY include/ DESTINATION /usr/local/include
    FILES_MATCHING PATTERN "*.h"
)
install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION /usr/local/lib)
