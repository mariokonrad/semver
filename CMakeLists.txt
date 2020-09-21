cmake_minimum_required(VERSION 3.17)

# undocumented, invisible by `--help-variable-list`
# mentioned here: https://www.reddit.com/r/cmake/comments/in73w8/build_out_of_source_automatically/g47dhdm/
#set(CMAKE_DISABLE_SOURCE_CHANGES ON)
#set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(semver
	VERSION 0.0.0
	DESCRIPTION "C++ semver and range"
	)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(DISABLE_TESTING "Disable Testing" OFF)
option(DISABLE_EXAMPLES "Disable Examples" OFF)

### library
add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME}
	PRIVATE
		include/semver/semver.hpp
		include/semver/range.hpp
		src/semver.cpp
		src/range.cpp
		src/detail/range_lexer.hpp
		src/detail/range_node.hpp
		src/detail/range_parser.hpp
		src/detail/semver_parser.hpp
	)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_compile_options(${PROJECT_NAME}
	PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		-Werror
		-pedantic-errors
		-Wold-style-cast
		-Wcast-align
		-Wcast-qual
		-Wmissing-field-initializers
		-Wmissing-format-attribute
		-Wmissing-include-dirs
		-Wmissing-noreturn
		-Wnon-virtual-dtor
		-Woverloaded-virtual
		-Wswitch
		-Wunreachable-code
		-Wunused
		-Wunused-parameter
		-Wvariadic-macros
		-Wwrite-strings
	)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION}
	)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(targets_export_name ${PROJECT_NAME}-targets)
set(include_install_dir ${CMAKE_INSTALL_INCLUDEDIR})

install(
	TARGETS ${PROJECT_NAME}
	EXPORT ${targets_export_name}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	INCLUDES DESTINATION ${include_install_dir}
	)

install(
	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
	DESTINATION ${include_install_dir}
	)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/config.cmake.in
	include(${CMAKE_CURRENT_LIST_DIR}/@targets_export_name@.cmake)
	)

configure_package_config_file(
	${CMAKE_CURRENT_BINARY_DIR}/config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
	PATH_VARS include_install_dir
	)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
	VERSION "${PROJECT_VERSION}" COMPATIBILITY SameMajorVersion
	)

install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
	DESTINATION
		${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
	)

### examples
if(NOT DISABLE_EXAMPLES)
	add_executable(semver-compare)
	target_sources(semver-compare PRIVATE example/semver-compare.cpp)
	target_link_libraries(semver-compare PRIVATE semver::semver)

	add_executable(range-parse)
	target_sources(range-parse PRIVATE example/range-parse.cpp)
	target_link_libraries(range-parse PRIVATE semver::semver)

	add_executable(range-check)
	target_sources(range-check PRIVATE example/range-check.cpp)
	target_link_libraries(range-check PRIVATE semver::semver)

	add_executable(range-normalize)
	target_sources(range-normalize PRIVATE example/range-normalize.cpp)
	target_link_libraries(range-normalize PRIVATE semver::semver)
endif()

### testing
if(NOT DISABLE_TESTING)
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

	enable_testing()
	add_executable(testrunner)

	target_sources(testrunner
		PRIVATE
			tests/unit/test_semver_construction.cpp
			tests/unit/test_semver_construction_loose.cpp
			tests/unit/test_semver_comparison.cpp
			tests/unit/test_semver_string.cpp
			tests/unit/test_semver_parser.cpp
			tests/unit/test_range_construction.cpp
			tests/unit/test_range_comparison.cpp
			#tests/unit/test_range_intersection.cpp # TODO: enable during implementation
			tests/unit/test_range_query.cpp
			tests/unit/test_range_node.cpp
			tests/unit/test_range_lexer.cpp
			tests/unit/test_range_bounds.cpp
			tests/unit/test_range_string.cpp
		)

	target_compile_options(testrunner
		PRIVATE
			-Wall
			-Wextra
			-Wpedantic
			-Werror
		)

	target_include_directories(testrunner
		PRIVATE
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
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
endif()

