from conan import ConanFile
import os

class SageStorePkg(ConanFile):
    name = "sage_store"
    version = "1.0.0"
    generators = "CMakeDeps", "CMakeToolchain", "VirtualBuildEnv", "VirtualRunEnv"

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    requires = [
        "qt/6.6.2",
        "spdlog/1.12.0",
        "nlohmann_json/3.11.2",
        "gtest/1.14.0",
        "sqlite3/3.45.0"
    ]
    default_options = {
        "shared": False,
        "boost/*:without_test": True,
        "sqlite3/*:threadsafe": 2
    }

    def requirements(self):
        if not self.is_boost_installed_manually():
            self.requires("boost/1.82.0")

    def is_boost_installed_manually(self):
        return "BOOST_ROOT" in os.environ

    def package_info(self):
        if self.is_boost_installed_manually():
            boost_root = os.environ['BOOST_ROOT']
            self.cpp_info.includedirs = [os.path.join(boost_root, "include")]
            self.cpp_info.libdirs = [os.path.join(boost_root, "lib")]
            self.env_info.BOOST_ROOT = boost_root