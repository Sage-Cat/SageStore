import os
from conan import ConanFile
from conan.tools.files import copy


class SageStorePkg(ConanFile):
    name = "sage_store"
    version = "1.0.0"
    generators = "CMakeDeps", "CMakeToolchain"

    settings = "os", "compiler", "build_type", "arch"
    implements = ["auto_shared_fpic"]

    # Define dependencies for client and server
    client_deps = ["spdlog"]
    server_deps = ["spdlog"]

    def requirements(self):
        self.requires("spdlog/[1.12.0]")
        # self.requires("cmake/[3.27.7]")
        # self.requires("qt/[6.4.2]")

    def layout(self):
        self.layouts.source = "."
        self.layouts.build = "build"

    def copy_dependency_files(self, dep, target_folders):
        dep_folder = dep.package_folder

        for target_folder in target_folders:
            if self.settings.os == "Windows":
                folder = os.path.join(dep_folder, dep.cpp_info.bindirs[0])
                copied_files = copy(self, "*.dll", folder, target_folder)
            else:
                folder = os.path.join(dep_folder, dep.cpp_info.libdirs[0])
                copied_files = copy(self, "*.so", folder, target_folder)

            copied_filenames = [os.path.basename(f) for f in copied_files]
            print(f"Copied files to {target_folder}: {copied_filenames}")

    def generate(self):
        for dep in self.dependencies.values():
            dep_name = dep.ref.name

            target_folders = []
            if dep_name in self.client_deps:
                target_folders.append(os.path.join(
                    self.build_folder, "_client"))
            if dep_name in self.server_deps:
                target_folders.append(os.path.join(
                    self.build_folder, "_server"))

            # Copy files for the current dependency
            self.copy_dependency_files(dep, target_folders)

            # Copy files for the dependencies of the current dependency
            for transitive_dep in dep.dependencies.values():
                self.copy_dependency_files(transitive_dep, target_folders)
