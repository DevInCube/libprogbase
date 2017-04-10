cmake_minimum_required(VERSION 2.8.7)
include_directories(include)

project (udpserver)
add_executable(${CMAKE_PROJECT_NAME} "udpserver.c")
target_link_libraries(${CMAKE_PROJECT_NAME} progbase)

project (udpclient)
add_executable(${CMAKE_PROJECT_NAME} "udpclient.c")
target_link_libraries(${CMAKE_PROJECT_NAME} progbase)

project (tcpserver)
add_executable(${CMAKE_PROJECT_NAME} "tcpserver.c")
target_link_libraries(${CMAKE_PROJECT_NAME} progbase)

project (tcpclient)
add_executable(${CMAKE_PROJECT_NAME} "tcpclient.c")
target_link_libraries(${CMAKE_PROJECT_NAME} progbase)
