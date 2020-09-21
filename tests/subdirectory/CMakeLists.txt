cmake_minimum_required(VERSION 3.17)
project(semver-subdir-test LANGUAGES CXX VERSION 0.0.0)

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE semver::semver)

set(DISABLE_TESTS    OFF CACHE BOOL "")
set(DISABLE_EXAMPLES OFF CACHE BOOL "")
add_subdirectory(${LIBRARY_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/semver)

