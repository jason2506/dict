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

        args = []
        args.append('-DENABLE_CONAN=%s' % self.options.enable_conan)
        args.append('-DBUILD_SHARED_LIBS=%s' % self.options.shared)
        args.append('-DBUILD_TESTING=%s' % self.options.build_tests)
        args.append('-DCMAKE_INSTALL_PREFIX="%s"' % self.package_folder)

        self.run('cmake "%s" %s %s' % (
            self.conanfile_directory,
            cmake.command_line,
            ' '.join(args)
        ))
        self.run('cmake --build .')

    def package(self):
        cmake = CMake(self.settings)
        self.run('cmake --build . --target install %s' % cmake.build_config)

    def package_info(self):
        self.cpp_info.libs = ['desa']
