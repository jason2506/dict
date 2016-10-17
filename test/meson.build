tests = [
  'bit_vector_test',
  'partial_sum_test',
  'rbtree_test',
  'wavelet_tree_test',
  'tree_list_test',
  'permutation_test',
  'text_index_test',
]

foreach test_name: tests
  exe = executable(test_name, test_name + '.cpp',
    link_with: lib,
    dependencies: gtest_dep,
  )
  test(test_name, exe,
    args: ['--gtest_color=yes'],
  )
endforeach
