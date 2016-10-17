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
subdir('test')
