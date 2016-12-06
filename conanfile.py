from conans import ConanFile, CMake


class DesaConan(ConanFile):

    name = 'desa'
    version = '0.1.0'
    url = 'https://github.com/jason2506/desa'
    license = 'BSD 3-Clause'
    author = 'Chi-En Wu'

    dev_requires = (
        'gtest/1.8.0@lasote/stable',
    )

    settings = ('os', 'compiler', 'build_type', 'arch')
    generators = ('cmake', 'txt', 'env')
    options = {
        'enable_conan': [True, False],
        'shared': [True, False],
    }
    default_options = (
        'gtest:shared=False',
        'enable_conan=True',
        'shared=False',
    )

    exports = (
        'CMakeLists.txt',
        'cmake/*.cmake',
        'include/*.hpp',
        'src/CMakeLists.txt',
        'src/*.cpp',
        'test/CMakeLists.txt',
        'test/*.cpp',
    )

    def build(self):
        extra_opts = []
        extra_opts.append('-DENABLE_CONAN={}'.format(
            self.options.enable_conan,
        ))
        extra_opts.append('-DBUILD_SHARED_LIBS={}'.format(
            self.options.shared,
        ))
        extra_opts.append('-DBUILD_TESTING={}'.format(
            bool(self.scope.dev and self.scope.build_tests),
        ))
        extra_opts.append('-DCMAKE_INSTALL_PREFIX="{}"'.format(
            self.package_folder,
        ))

        cmake = CMake(self.settings)
        self.run('cmake "{src_dir}" {opts} {extra_opts}'.format(
            src_dir=self.conanfile_directory,
            opts=cmake.command_line,
            extra_opts=' '.join(extra_opts),
        ))
        self.run('cmake --build . {}'.format(cmake.build_config))

    def package(self):
        self.run('cmake --build . --target install')

    def package_info(self):
        self.cpp_info.libs = ['desa']
