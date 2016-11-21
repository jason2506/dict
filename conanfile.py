from conans import ConanFile, CMake


class DesaConan(ConanFile):

    name = 'desa'
    version = '0.1.0'
    url = 'https://github.com/jason2506/desa'
    license = 'BSD 3-Clause'
    author = 'Chi-En Wu'

    settings = 'os', 'compiler', 'build_type', 'arch'
    generators = 'cmake'
    options = {
        'enable_conan': [True, False],
        'shared': [True, False],
        'build_tests': [True, False],
    }
    default_options = (
        'gtest:shared=False',
        'enable_conan=True',
        'shared=False',
        'build_tests=True',
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

    def requirements(self):
        if self.options.build_tests:
            self.requires('gtest/1.8.0@lasote/stable')

    def build(self):
        cmake = CMake(self.settings)
        self.run(
            'cmake "%s" %s'
            ' -DENABLE_CONAN=%s'
            ' -DBUILD_SHARED_LIBS=%s'
            ' -DBUILD_TESTING=%s' % (
                self.conanfile_directory,
                cmake.command_line,
                self.options.enable_conan,
                self.options.shared,
                self.options.build_tests,
            )
        )
        self.run('cmake --build . %s' % cmake.build_config)

    def package(self):
        self.copy('*.cmake', dst='', src='cmake')
        self.copy('*.hpp', dst='include', src='include')
        self.copy('*.a', dst='lib', src='lib')
        self.copy('*.lib', dst='lib', src='lib')
        self.copy('*.so*', dst='lib', src='lib')
        self.copy('*.dylib*', dst='lib', src='lib')

    def package_info(self):
        self.cpp_info.libs = ['desa']
