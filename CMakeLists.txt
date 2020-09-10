cmake_minimum_required(VERSION 3.17)

# undocumented, invisible by `--help-variable-list`
# mentioned here: https://www.reddit.com/r/cmake/comments/in73w8/build_out_of_source_automatically/g47dhdm/
#set(CMAKE_DISABLE_SOURCE_CHANGES ON)
#set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(semver)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(EXISTS $ENV{HOME}/local/repo/googletest)
	set(repo_googletest "file://$ENV{HOME}/local/repo/googletest")
else()
	set(repo_googletest "https://github.com/google/googletest.git")
endif()

include(FetchContent)

set(BUILD_GMOCK TRUE CACHE BOOL "" FORCE) # do not repeat option, because of description
FetchContent_Declare(googletest
	GIT_REPOSITORY ${repo_googletest}
	GIT_TAG release-1.10.0
	)
FetchContent_MakeAvailable(googletest)

### library
add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME}
	INTERFACE
		include/semver/semver.hpp
		include/semver/range.hpp
		include/semver/detail/range_lexer.hpp
	)
target_include_directories(${PROJECT_NAME}
	INTERFACE
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

### examples
add_executable(semver-compare)
target_sources(semver-compare PRIVATE example/semver-compare.cpp)
target_link_libraries(semver-compare PRIVATE semver)

add_executable(range-parse)
target_sources(range-parse PRIVATE example/range-parse.cpp)
target_link_libraries(range-parse PRIVATE semver)

add_executable(range-check)
target_sources(range-check PRIVATE example/range-check.cpp)
target_link_libraries(range-check PRIVATE semver)

add_executable(range-normalize)
target_sources(range-normalize PRIVATE example/range-normalize.cpp)
target_link_libraries(range-normalize PRIVATE semver)

### testing
enable_testing()
add_executable(testrunner)
target_sources(testrunner
	PRIVATE
		tests/test_semver_construction.cpp
		tests/test_semver_construction_loose.cpp
		tests/test_semver_comparison.cpp
		tests/test_semver_string.cpp
		tests/test_range_construction.cpp
		tests/test_range_comparison.cpp
		tests/test_range_intersection.cpp
		tests/test_range_query.cpp
		tests/test_range_node.cpp
		tests/test_range_lexer.cpp
		tests/test_range_bounds.cpp
		tests/test_range_string.cpp
	)
target_compile_options(testrunner
	PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		-Werror
	)
target_link_libraries(testrunner
	PRIVATE
		gtest
		gmock
		gmock_main
		${PROJECT_NAME}
	)
include(GoogleTest)
gtest_add_tests(TARGET testrunner)

