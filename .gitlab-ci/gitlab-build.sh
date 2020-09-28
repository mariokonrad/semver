#!/usr/bin/env bash
set -eu

function section()
{
	id=$1
	shift

	echo -e "section_start:`date +%s`:${id}\r\e[0K${id}"
	eval $@
	echo -e "section_end:`date +%s`:${id}\r\e[0K"
}

section configure cmake -B build -DCMAKE_BUILD_TYPE=${BUILD_TYPE} .
section build     cmake --build build -j
section unittest  cmake --build build --target test -- ARGS="--verbose"
section tests     "(cd build ; ctest)"

