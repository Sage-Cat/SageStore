import os
from conan import ConanFile
from conan.tools.files import copy


class SageStorePkg(ConanFile):
    name = "sage_store"
    version = "1.0.0"
    generators = "CMakeDeps", "CMakeToolchain"

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {
        "shared": False,
        "boost/*:without_test": True
    }

    def requirements(self):
        self.requires("boost/[1.82.0]")
        self.requires("spdlog/[1.12.0]")
        self.requires("gtest/[1.14.0]")
