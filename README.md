# DICT - Dynamic Indexing for Collection of Text


## Instructions

### Requirements

- [CMake](https://cmake.org) >= 3.1
- C++ compiler which supports features of C++14

### Installing with Conan (and CMake)

The recommended way to use _DICT_ package in your project is to install the package with Conan.

Assume that your project is built with CMake, you can just execute the following command in your __build directory__:

```sh
$ conan install dict/0.1.0@jason2506/testing -b outdated -g cmake
```

The `install` command will download the package (together with its dependencies) and generate `conanbuildinfo.cmake` file in the current directory.

Additionally, you need to include `conanbuildinfo.cmake` and then add `conan_basic_setup()` command into your `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.1)
project(myproj)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
```

This will setup necessary CMake variables for finding installed libraries and related files.

Now, you can use `find_package()` and `target_link_libraries()` commands to locate and link the package. For example,

```cmake
find_package(dict)

if(dict_FOUND)
    add_executable(myexec mycode.cpp)
    target_link_libraries(myexec dict)
endif()
```

The final step is to build your project with CMake, like:

```sh
$ cmake [SOURCE_DIR] -DCMAKE_BUILD_TYPE=Release
$ cmake --build .
```

Please check [conan docs](http://docs.conan.io/en/latest/) for more details about how to use conan packages, generators and much more.

### Installing without Conan

If you do not intend to use Conan in your project, you can just clone this repository and manually build/install the package through following commands:

```sh
$ cmake -H. -B_build -DCMAKE_BUILD_TYPE=Release
$ cmake --build _build
```

which will create a `_build/` directory and then build the package inside it.

After compiling the package, you can just copy necessary files (e.g., header files and libraries) into your project, or automatically install the package with `install` build target:

```sh
$ cmake --build _build --target install
```

### Building Tests

One way of building tests is to execute `conan` commands with `--scope build_tests=True` option.

Here's an example:

```sh
$ mkdir _build && cd _build
$ conan install .. --build outdated --scope build_tests=True
$ conan build ..
```

Alternatively, you can install [Google Test](https://github.com/google/googletest) yourself, setup CMake variables for finding that, and enable `BUILD_TESTING` option:

```sh
$ cmake -H. -B_build -DCMAKE_BUILD_TYPE=Release \
     -DGTEST_ROOT=... \
     -DBUILD_TESTING=ON
$ cmake --build _build
```

Now, all test executables are placed in the `_build` directory. You can then execute the tests with `test` build target:

```sh
$ cmake --build _build --target test
```


## References

- F. Claude, G. Navarro, and A. Ordóñez, "The wavelet matrix: An efficient wavelet tree for large alphabets," _Information Systems_, vol. 47, pp. 15–32, 2015.
- W. Gerlach, "Dynamic FM-Index for a collection of texts with application to space-efficient construction of the compressed suffix array," Master’s thesis, Universität Bielefeld, Germany, 2007.
- M. Salson, T. Lecroq, M. Léonard, and L. Mouchard, "A four-stage algorithm for updating a Burrows–Wheeler transform," _Theoretical Computer Science_, vol. 410, no. 43, pp. 4350–4359, 2009.
- M. Salson, T. Lecroq, M. Léonard, and L. Mouchard, "Dynamic extended suffix arrays," _Journal of Discrete Algorithms_, vol. 8, no. 2, pp. 241–257, 2010.


## License

Copyright (c) 2015-2017, Chi-En Wu.

Distributed under [The BSD 3-Clause License](http://opensource.org/licenses/BSD-3-Clause).
