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
        # "wolfssl/*:opensslextra": True
    }

    def requirements(self):
        # Core dependencies for the project
        self.requires("spdlog/1.12.0")
        self.requires("nlohmann_json/3.11.2")
        self.requires("boost/1.82.0")

        # Security library (custom build options might be applied in its recipe)
        # self.requires("wolfssl/5.6.6")

    def build_requirements(self):
        # GTest only needed for testing, not runtime
        self.tool_requires("gtest/1.14.0")
        pass
