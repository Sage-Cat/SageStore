from conan import ConanFile
import os

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
        if not IS_BOOST_INSTASLLED_HANDLY():
            self.requires("boost/1.82.0") #Not needed if installed handly
        self.requires("gtest/1.14.0")
        self.requires("sqlite3/3.45.1")

# Check is Boost installed handly
def IS_BOOST_INSTASLLED_HANDLY():
    IS_BOOST_INSTALLED = False
    
    # Check BOOST_ROOT
    if "BOOST_ROOT" in os.environ:
        IS_BOOST_INSTALLED = True

    # Check BOOST in PATH
    path_list = os.environ.get('PATH', '').split(';')
    for path in path_list:
        if 'boost' in path.lower():
            IS_BOOST_INSTALLED = True
            break
    
    return IS_BOOST_INSTALLED
