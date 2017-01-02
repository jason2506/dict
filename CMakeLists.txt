cmake_minimum_required(VERSION 3.1)

project(DICT
    VERSION 0.1.0
    LANGUAGES C CXX
)

option(ENABLE_CONAN "Enable conan package manager" OFF)
if(ENABLE_CONAN)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
endif(ENABLE_CONAN)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(${PROJECT_NAME}_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
install(DIRECTORY ${${PROJECT_NAME}_INCLUDE_DIR}/
    DESTINATION include
    PATTERN *.hpp
)

add_subdirectory(src)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif(BUILD_TESTING)

include(cmake/DICTPackage.cmake)
