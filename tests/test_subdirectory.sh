#!/bin/bash -e

SCRIPT_DIR=$(dirname $(readlink -f "$0"))
LIBRARY_REPOSITORY=${SCRIPT_DIR}/..

build_dir=$(mktemp -d)

cmake \
	-B ${build_dir} \
	-DCMAKE_BUILD_TYPE=Release \
	-DLIBRARY_REPOSITORY=${LIBRARY_REPOSITORY} \
	${SCRIPT_DIR}/subdirectory

cmake --build ${build_dir}

${build_dir}/semver-test && echo "success"

rm -fr ${build_dir}

