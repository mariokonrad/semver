cmake_minimum_required(VERSION 3.21)
project(semver-test LANGUAGES CXX VERSION 0.0.0)

include(FetchContent)
set(DISABLE_EXAMPLES TRUE CACHE BOOL "" FORCE)
set(DISABLE_TESTING  TRUE CACHE BOOL "" FORCE)
FetchContent_Declare(semver GIT_REPOSITORY ${LIBRARY_REPOSITORY})
FetchContent_MakeAvailable(semver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE semver::semver)

