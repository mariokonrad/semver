from conans import ConanFile, CMake, tools
import os, re


class Package(ConanFile):
    license = "BSD"
    description = "C++ library implementing semver 2.0.0 and partial support of ranges of node-semver"
    url = "https://gitlab.com/mariokonrad/semver"

    settings = "os", "arch", "compiler", "build_type"

    options = {"shared": [True, False]}
    default_options = {"shared": False}

    scm = {
        'type': 'git',
        'url': 'auto',
        'revision': 'auto',
    }

    __cmake = None

    def __version_info(self):
        git = tools.Git(folder=self.recipe_folder)
        tag_with_dirty = git.run('describe --tags --dirty')
        last_tag = git.run('describe --tags --abbrev=0')
        counts_commits = git.run(f'rev-list {last_tag}..HEAD --count')
        commit_hash = git.run('rev-parse --short HEAD')

        p = re.compile('.*-.*')
        if p.match(last_tag):
            info = f'{last_tag}.{counts_commits}.{commit_hash}'
        else:
            info = f'{last_tag}-{counts_commits}.{commit_hash}'

        semver_pattern = re.compile("^v?([0-9]+)\.([0-9]+)\.([0-9]+)(-[a-zA-Z0-9.-]+)?(\\+[a-zA-Z0-9.-]+)?$")
        v = semver_pattern.search(info)

        major = v.group(1)
        minor = v.group(2)
        patch = v.group(3)
        tweak = v.group(4)

        if (int(counts_commits) > 0) or (re.match('.*-dirty.*', tag_with_dirty)):
            version = f'{major}.{minor}.{patch}{tweak}'
        else:
            version = f'{major}.{minor}.{patch}'

        return version

    def __cmake_info(self):
        cmakelists = tools.load(os.path.join(self.recipe_folder, 'CMakeLists.txt'))
        result = re.search(r'project\s*\(\s*([a-zA-Z0-9_-]+).*\)', cmakelists, flags=re.DOTALL)
        return result.group(1)

    def __setup_cmake(self):
        if not self.__cmake:
            self.__cmake = CMake(self)
            self.__cmake.definitions['DISABLE_TESTING'] = "TRUE"
            self.__cmake.definitions['DISABLE_EXAMPLES'] = "TRUE"
            self.__cmake.configure()
        return self.__cmake

    def set_name(self):
        self.name = self.__cmake_info()

    def set_version(self):
        self.version = self.__version_info()

    def build(self):
        cmake = self.__setup_cmake()
        cmake.build()

    def package(self):
        cmake = self.__setup_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = [self.name]

