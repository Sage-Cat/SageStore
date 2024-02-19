from conan import ConanFile


class SageStorePkg(ConanFile):
    name = "sage_store"
    version = "1.0.0"
    generators = "CMakeDeps", "CMakeToolchain", "VirtualBuildEnv", "VirtualRunEnv"

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {
        "shared": False,
        "boost/*:without_test": True,
        "sqlite3/*:threadsafe": 2
    }

    def requirements(self):
        self.requires("spdlog/1.12.0")
        self.requires("nlohmann_json/3.11.2")
        self.requires("boost/1.82.0")
        self.requires("gtest/1.14.0")
        self.requires("sqlite3/3.45.1")
