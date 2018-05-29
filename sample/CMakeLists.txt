cmake_minimum_required(VERSION 2.8.7)

project(basic-console)
add_executable(${PROJECT_NAME} console.c)
target_link_libraries(${PROJECT_NAME} progbase)

project(conGetChar)
add_executable(${PROJECT_NAME} conGetChar.c)
target_link_libraries(${PROJECT_NAME} progbase)

project(conIsKeyDown)
add_executable(${PROJECT_NAME} conIsKeyDown.c)
target_link_libraries(${PROJECT_NAME} progbase)

project(conLock)
add_executable(${PROJECT_NAME} conLock.c)
target_link_libraries(${PROJECT_NAME} progbase)

project(conLockFire)
add_executable(${PROJECT_NAME} conLockFire.c)
target_link_libraries(${PROJECT_NAME} progbase)

project(conResize)
add_executable(${PROJECT_NAME} conResize.c)
target_link_libraries(${PROJECT_NAME} progbase)

project(collections-pbvector)
add_executable(${PROJECT_NAME} collections/pbvector.c)
target_link_libraries(${PROJECT_NAME} progbase)