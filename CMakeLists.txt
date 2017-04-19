cmake_minimum_required(VERSION 2.8.7)

include_directories(include)
file(GLOB LIB_SOURCES "src/*.c")

project (progbase)
add_library(${CMAKE_PROJECT_NAME} STATIC ${LIB_SOURCES})
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY C_STANDARD 99)

install(
    DIRECTORY include/ DESTINATION /usr/local/include
    FILES_MATCHING PATTERN "*.h"
)
install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION /usr/local/lib)

SET(CMAKE_C_FLAGS "-std=c99 -Werror -Wall -pedantic-errors")
