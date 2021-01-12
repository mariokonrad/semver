#!/bin/bash -e

echo -e "section_start:`date +%s`:$(basename $0)\r\e[0K$(basename $0)"

SCRIPT_DIR=$(dirname $(readlink -f "$0"))
LIBRARY_REPOSITORY=${SCRIPT_DIR}/..

build_dir=$(mktemp -d)

export CONAN_USER_HOME=${build_dir}

conan create \
	-s compiler=gcc \
	-s compiler.version=9 \
	-s compiler.libcxx=libstdc++11 \
	-e CXX=g++-9 \
	${LIBRARY_REPOSITORY} \
	local/testing

conan install \
	--install-folder=${build_dir}/local \
	-s compiler=gcc \
	-s compiler.version=9 \
	-s compiler.libcxx=libstdc++11 \
	-e CXX=g++-9 \
	${SCRIPT_DIR}/conan-gen

cmake \
	-B ${build_dir}/build \
	-DCMAKE_MODULE_PATH=${build_dir}/local \
	-DCMAKE_CXX_COMPILER=g++-9 \
	-DCMAKE_BUILD_TYPE=Release \
	${SCRIPT_DIR}/conan-gen

cmake --build ${build_dir}/build

${build_dir}/build/semver-test && echo "success"

rm -fr ${build_dir}

echo -e "section_end:`date +%s`:$(basename $0)\r\e[0K"

