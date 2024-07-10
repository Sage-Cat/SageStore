from conan import ConanFile
import os

class SageStorePkg(ConanFile):
    name = "sage_store"
    version = "1.0.0"
    generators = "CMakeDeps", "CMakeToolchain", "VirtualBuildEnv", "VirtualRunEnv"

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    requires = [
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
        if not self.is_qt_installed_manually():
            self.requires("qt/6.6.2")

    def is_boost_installed_manually(self):
        return "BOOST_ROOT" in os.environ

    def is_qt_installed_manually(self):
        return "QT_DIR" in os.environ

    def package_info(self):
        super().package_info()
        if self.is_boost_installed_manually():
            boost_root = os.environ['BOOST_ROOT']
            self.cpp_info.includedirs.append(os.path.join(boost_root, "include"))
            self.cpp_info.libdirs.append(os.path.join(boost_root, "lib"))
            self.env_info.BOOST_ROOT = boost_root
        if self.is_qt_installed_manually():
            qt_dir = os.environ['QT_DIR']
            self.cpp_info.includedirs.append(os.path.join(qt_dir, "include"))
            self.cpp_info.libdirs.append(os.path.join(qt_dir, "lib"))
            self.env_info.QT_DIR = qt_dir