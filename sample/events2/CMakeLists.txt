cmake_minimum_required(VERSION 2.8.7)
include_directories(../../include)

SET(CMAKE_С_FLAGS "-fsanitize=address")

project (basic)
add_executable(${PROJECT_NAME} "basic.c")
target_link_libraries(${PROJECT_NAME} progbase)

project (basic_input)
add_executable(${CMAKE_PROJECT_NAME} "basic_input.c")
target_link_libraries(${PROJECT_NAME} progbase)

project (many_handlers)
add_executable(${CMAKE_PROJECT_NAME} "many_handlers.c")
target_link_libraries(${PROJECT_NAME} progbase)
