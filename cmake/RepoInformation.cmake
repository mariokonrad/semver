find_package(Git)

if(NOT GIT_FOUND)
	message(FATAL_ERROR "git not found, no further version info available")
endif()


function(project_version_from_git dir)
	if(NOT EXISTS ${dir}/.git)
		message(FATAL_ERROR "not a git repository, version information not available")
	endif()

	execute_process(
		COMMAND ${GIT_EXECUTABLE} describe --tags --dirty
		WORKING_DIRECTORY "${dir}"
		OUTPUT_VARIABLE tag_with_dirty
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
		WORKING_DIRECTORY "${dir}"
		OUTPUT_VARIABLE last_tag
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} rev-list ${last_tag}..HEAD --count
		WORKING_DIRECTORY "${dir}"
		OUTPUT_VARIABLE count_commits
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
		WORKING_DIRECTORY "${dir}"
		OUTPUT_VARIABLE commit_hash
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)

	if(last_tag MATCHES ".*-.*")
		set(info "${last_tag}.${count_commits}.${commit_hash}")
	else()
		set(info "${last_tag}-${count_commits}.${commit_hash}")
	endif()

	if(tag_with_dirty MATCHES ".*-dirty.*")
		string(APPEND info "-dirty")
	endif()

	set(semver_pattern "^v?([0-9]+)\.([0-9]+)\.([0-9]+)(-[a-zA-Z0-9.-]+)?(\\+[a-zA-Z0-9.-]+)?$")

	string(REGEX MATCH "${semver_pattern}" tmp "${info}")
	set(PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1})

	string(REGEX MATCH "${semver_pattern}" tmp "${info}")
	set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_2})

	string(REGEX MATCH "${semver_pattern}" tmp "${info}")
	set(PROJECT_VERSION_PATCH ${CMAKE_MATCH_3})

	string(REGEX MATCH "${semver_pattern}" tmp "${info}")
	if(CMAKE_MATCH_4)
		set(PROJECT_VERSION_TWEAK ${CMAKE_MATCH_4})
	endif()

	string(REGEX MATCH "${semver_pattern}" tmp "${info}")
	if(CMAKE_MATCH_5)
		set(PROJECT_VERSION_BUILD ${CMAKE_MATCH_5})
	endif()

	if((count_commits GREATER 0) OR (tag_with_dirty MATCHES ".*-dirty.*"))
		set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}${PROJECT_VERSION_TWEAK}")
	else()
		set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
	endif()

	set(PROJECT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} PARENT_SCOPE)
	set(PROJECT_VERSION_MINOR ${PROJECT_VERSION_MINOR} PARENT_SCOPE)
	set(PROJECT_VERSION_PATCH ${PROJECT_VERSION_PATCH} PARENT_SCOPE)
	set(PROJECT_VERSION_TWEAK ${PROJECT_VERSION_TWEAK} PARENT_SCOPE)
	set(PROJECT_VERSION       ${PROJECT_VERSION}       PARENT_SCOPE)
	set(PROJECT_VERSION_BUILD ${PROJECT_VERSION_BUILD} PARENT_SCOPE)
endfunction()

