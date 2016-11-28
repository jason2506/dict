# Define `IMPORTED` targets `GTest::GTest` and `GTest::Main` for CMake < 3.5.
# Extract from: https://github.com/Kitware/CMake/blob/86578e/Modules/FindGTest.cmake

include(CMakeFindDependencyMacro)
find_dependency(Threads)

if(NOT TARGET GTest::GTest)
    add_library(GTest::GTest UNKNOWN IMPORTED)
    set_target_properties(GTest::GTest PROPERTIES
        INTERFACE_LINK_LIBRARIES "Threads::Threads")

    if(GTEST_INCLUDE_DIRS)
        set_target_properties(GTest::GTest PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIRS}")
    endif()

    if(EXISTS "${GTEST_LIBRARY}")
        set_target_properties(GTest::GTest PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
            IMPORTED_LOCATION "${GTEST_LIBRARY}")
    endif()

    if(EXISTS "${GTEST_LIBRARY_RELEASE}")
        set_property(TARGET GTest::GTest APPEND PROPERTY
            IMPORTED_CONFIGURATIONS RELEASE)
        set_target_properties(GTest::GTest PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
            IMPORTED_LOCATION_RELEASE "${GTEST_LIBRARY_RELEASE}")
    endif()

    if(EXISTS "${GTEST_LIBRARY_DEBUG}")
        set_property(TARGET GTest::GTest APPEND PROPERTY
            IMPORTED_CONFIGURATIONS DEBUG)
        set_target_properties(GTest::GTest PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
            IMPORTED_LOCATION_DEBUG "${GTEST_LIBRARY_DEBUG}")
    endif()
endif()

if(NOT TARGET GTest::Main)
    add_library(GTest::Main UNKNOWN IMPORTED)
    set_target_properties(GTest::Main PROPERTIES
        INTERFACE_LINK_LIBRARIES "GTest::GTest")

    if(EXISTS "${GTEST_MAIN_LIBRARY}")
        set_target_properties(GTest::Main PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
            IMPORTED_LOCATION "${GTEST_MAIN_LIBRARY}")
    endif()

    if(EXISTS "${GTEST_MAIN_LIBRARY_RELEASE}")
        set_property(TARGET GTest::Main APPEND PROPERTY
            IMPORTED_CONFIGURATIONS RELEASE)
        set_target_properties(GTest::Main PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
            IMPORTED_LOCATION_RELEASE "${GTEST_MAIN_LIBRARY_RELEASE}")
    endif()

    if(EXISTS "${GTEST_MAIN_LIBRARY_DEBUG}")
        set_property(TARGET GTest::Main APPEND PROPERTY
            IMPORTED_CONFIGURATIONS DEBUG)
        set_target_properties(GTest::Main PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
            IMPORTED_LOCATION_DEBUG "${GTEST_MAIN_LIBRARY_DEBUG}")
    endif()
endif()
