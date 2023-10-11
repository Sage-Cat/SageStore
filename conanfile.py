from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain

class SageStoreClientConan(ConanFile):
    name = "SageStoreClient"
    version = "1.0.0"
    generators = "CMakeDeps"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}

    # Specify required packages
    def requirements(self):
        # self.requires("qt/[6.4.3]") 
        self.requires("spdlog/[1.12.0]") 
        # self.requires("boost/[*]")
        # self.requires("sqlite3/[*]") 
        # self.requires("nlohmann_json/[*]")
 
    # Build configurations using CMake
    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
