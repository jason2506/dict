project('desa', 'cpp',
  version: '0.1.0',
  license: 'BSD 3-Clause',
  default_options: [
    'cpp_std=c++11',
    'default_library=static',
  ]
)

gtest_sp = subproject('gtest')
gtest_dep = gtest_sp.get_variable('gtest_main_dep')

subdir('src')

inc = include_directories('include')
dep = declare_dependency(
  include_directories: inc,
  link_with: lib,
)

subdir('test')
