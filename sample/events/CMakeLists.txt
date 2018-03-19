cmake_minimum_required(VERSION 2.8.7)
include_directories(../../include)

SET(CMAKE_С_FLAGS "-fsanitize=address")
file(GLOB SOURCES  
    "../../src/progbase.c" 
    "../../src/clock.c" 
    "../../src/events.c" 
    "../../src/linux/console.c"
    "../../src/collections/queue.c" 
    "../../src/collections/list.c")

project (basic)
add_executable(${CMAKE_PROJECT_NAME} "basic.c" ${SOURCES})
# target_link_libraries(${CMAKE_PROJECT_NAME} progbase)

project (basic_input)
add_executable(${CMAKE_PROJECT_NAME} "basic_input.c" ${SOURCES})
# target_link_libraries(${CMAKE_PROJECT_NAME} progbase)

project (many_handlers)
add_executable(${CMAKE_PROJECT_NAME} "many_handlers.c" ${SOURCES})
# target_link_libraries(${CMAKE_PROJECT_NAME} progbase)
