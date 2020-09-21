#!/bin/bash -e

SCRIPT_DIR=$(dirname $(readlink -f "$0"))
LIBRARY_SOURCE_DIR=${SCRIPT_DIR}/..

build_dir=$(mktemp -d)

cmake \
	-B ${build_dir} \
	-DCMAKE_BUILD_TYPE=Release \
	-DDISABLE_EXAMPLES=TRUE \
	-DDISABLE_TESTING=TRUE \
	-DLIBRARY_SOURCE_DIR=${LIBRARY_SOURCE_DIR} \
	${SCRIPT_DIR}/subdirectory

cmake --build ${build_dir}

${build_dir}/semver-subdir-test && echo "success"

rm -fr ${build_dir}

