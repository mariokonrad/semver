cmake_minimum_required(VERSION 3.21)

project(semver
	DESCRIPTION "C++ library implementing semver 2.0.0 and partial support of ranges of node-semver"
	)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(RepoInformation)
project_version_from_git(${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Project Version: ${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(DISABLE_TESTING  "Disable Testing"  OFF)
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
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wextra>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wpedantic>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Werror>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-pedantic-errors>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wold-style-cast>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wcast-align>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wcast-qual>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wmissing-field-initializers>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wmissing-format-attribute>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wmissing-include-dirs>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wmissing-noreturn>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wnon-virtual-dtor>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Woverloaded-virtual>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wswitch>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wunreachable-code>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wunused>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wunused-parameter>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wvariadic-macros>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wwrite-strings>
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

install(
	EXPORT ${targets_export_name}
	NAMESPACE "${PROJECT_NAME}::"
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
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

	if(NOT DISABLE_EXAMPLES)
		function(add_test_normalize name str pattern)
			add_test(NAME normalize-${name} COMMAND $<TARGET_FILE:range-normalize> ${str})
			set_tests_properties(normalize-${name} PROPERTIES PASS_REGULAR_EXPRESSION ${pattern})
		endfunction()

		add_test_normalize(x-range-major "x"     ">=0\.0\.0")
		add_test_normalize(x-range-minor "1.x"   ">=1\.0\.0 <2\.0\.0")
		add_test_normalize(x-range-patch "1.2.x" ">=1\.2\.0 <1\.3\.0-0")
	endif()

	# unit tests with GoogleMock
	if(EXISTS $ENV{HOME}/local/repo/googletest)
		set(repo_googletest "file://$ENV{HOME}/local/repo/googletest")
	else()
		set(repo_googletest "https://github.com/google/googletest.git")
	endif()

	include(FetchContent)

	set(BUILD_GMOCK TRUE CACHE BOOL "" FORCE) # do not repeat option, because of description
	FetchContent_Declare(googletest
		GIT_REPOSITORY ${repo_googletest}
		GIT_TAG release-1.11.0
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
			tests/unit/test_range_query.cpp
			tests/unit/test_range_node.cpp
			tests/unit/test_range_lexer.cpp
			tests/unit/test_range_bounds.cpp
			tests/unit/test_range_string.cpp
		)

	target_compile_options(testrunner
		PRIVATE
			$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall>
			$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wextra>
			$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wpedantic>
			$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Werror>
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

