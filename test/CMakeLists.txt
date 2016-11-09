find_package(GTest REQUIRED)

set(${PROJECT_NAME}_TESTS
    rbtree_test
    bit_vector_test
    partial_sum_test
    wavelet_tree_test
    permutation_test
    tree_list_test
    text_index_test
)

enable_testing()
foreach(test ${${PROJECT_NAME}_TESTS})
    add_executable(${test} ${test}.cpp)

    target_link_libraries(${test}
        GTest::GTest
        GTest::Main
        ${PROJECT_NAME}
    )

    add_test(NAME ${test}
        COMMAND ${test} --gtest_color=yes
    )
endforeach(test ${test_SRCS})
