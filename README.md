# SEMVER

(C) 2020 Mario Konrad

License: BSD, see [LICENSE.md](LICENSE.md)

## Introduction

Implementation of semver 2.0.0, https://semver.org/

Parital implementatoin of ranges, https://github.com/npm/node-semver#versions, https://semver.npmjs.com/
- support for hyphen ranges, e.g. `1.2.3 - 2.0.0`, `1.2 - 2`
- support for wildcards, e.g. `1.2.x`, `1.2`, `1.X.3`, `*.2.3`
- support for tile ranges, e.g. `~1.2.3`, `~1.2`
- support for caret ranges, e.g. `^1.2.3`

## Examples `semver`

Construction from string:
```cpp
auto v = semver("1.2.3-pre.1+build.2");
```

Construction with numerical values:
```cpp
auto v = semver(1, 2, 3, "pre.1", "build.2");
```

Comparison of versions:
```cpp
auto v1 = semver("1.2.3-pre.1+build.2");
auto v2 = semver(1, 2, 3, "pre.1", "build.2");
assert(v1 == v2);

auto v3 = semver("1.2.3");
auto v4 = semver("2.3.4");
assert(v3 < v4);
```

## Examples `range`

Construction
```cpp
const auto r = range(">=1.2.3 <2.0.0 || 3.0.0");
```

Testing versions against ranges:
```cpp
const auto r = range(">=1.2.3 <2.0.0 || 3.0.0");
assert(r.satisfies(semver("1.2.3")));
assert(r.satisfies(semver("1.9.0")));
assert(r.satisfies(semver("3.0.0")));
```

## Build

The build is CMake based. It requires a C++17 capable compiler.
GoogleTest is used for testing.

Tested with compilers:
- GCC-9
- GCC-10
- Clang-9
- Clang-10
- Clang-12

Build and run tests:
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release .
cmake --build build -j 4
build/testrunner
```

