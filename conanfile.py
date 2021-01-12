from conans import ConanFile, CMake, tools
import os, re


class Package(ConanFile):
    license = "BSD"
    description = "C++ library implementing semver 2.0.0 and partial support of ranges of node-semver"
    url = "https://gitlab.com/mariokonrad/semver"

    settings = "os", "arch", "compiler", "build_type"

    options = {"shared": [True, False]}
    default_options = {"shared": False}

    exports_sources = 'LICENSE.md', 'CMakeLists.txt', 'include/*', 'src/*'

    __cmake = None

    def __cmake_info(self):
        cmakelists = tools.load(os.path.join(self.recipe_folder, 'CMakeLists.txt'))
        result = re.search(r'project\s*\(\s*([a-zA-Z0-9-_]+)\s+\bVERSION\b\s+(\d+\.\d+\.\d+)(.|\s)*\)',
                           cmakelists)
        return result.group(1), result.group(2)

    def __setup_cmake(self):
        if not self.__cmake:
            self.__cmake = CMake(self)
            self.__cmake.definitions['DISABLE_TESTING'] = "TRUE"
            self.__cmake.definitions['DISABLE_EXAMPLES'] = "TRUE"
            self.__cmake.configure()
        return self.__cmake

    def set_name(self):
        name, version = self.__cmake_info()
        self.name = name

    def set_version(self):
        name, version = self.__cmake_info()
        self.version = version

    def build(self):
        cmake = self.__setup_cmake()
        cmake.build()

    def package(self):
        cmake = self.__setup_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ['semver']

