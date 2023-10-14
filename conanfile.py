import os
from conan import ConanFile
from conan.tools.files import copy


class SageStorePkg(ConanFile):
    name = "sage_store"
    version = "1.0.0"
    generators = "CMakeDeps", "CMakeToolchain"

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}

    client_deps = ["spdlog", "gtest"]
    server_deps = ["spdlog", "gtest"]

    def requirements(self):
        self.requires("spdlog/[1.12.0]")
        self.requires("gtest/[1.14.0]")

    def layout(self):
        self.layouts.source = "."
        self.layouts.build = "build"

    def _copy_dependency_files(self, dep, target_folders, include_gtest=False, copied_files_dict=None):
        dep_name = dep.ref.name
        if not include_gtest and dep_name == "gtest":
            return

        dep_folder = dep.package_folder
        for target_folder in target_folders:
            if self.settings.os == "Windows":
                folder = os.path.join(dep_folder, dep.cpp_info.bindirs[0])
                copied_files = copy(self, "*.dll", folder, target_folder)
            else:
                folder = os.path.join(dep_folder, dep.cpp_info.libdirs[0])
                copied_files = copy(self, "*.so", folder, target_folder)

            copied_filenames = [os.path.basename(f) for f in copied_files]
            copied_files_dict[target_folder].extend(copied_filenames)

    def generate(self):
        client_target_folder = os.path.join(self.build_folder, "_client")
        server_target_folder = os.path.join(self.build_folder, "_server")

        client_unit_test_target_folder = os.path.join(
            client_target_folder, "tests/unit")
        server_unit_test_target_folder = os.path.join(
            server_target_folder, "tests/unit")

        copied_files_dict = {
            client_target_folder: [],
            server_target_folder: [],
            client_unit_test_target_folder: [],
            server_unit_test_target_folder: []
        }

        for dep in self.dependencies.values():
            dep_name = dep.ref.name

            if dep_name in self.client_deps:
                self._copy_dependency_files(
                    dep, [client_target_folder], copied_files_dict=copied_files_dict)
                self._copy_dependency_files(
                    dep, [client_unit_test_target_folder], include_gtest=True, copied_files_dict=copied_files_dict)

            if dep_name in self.server_deps:
                self._copy_dependency_files(
                    dep, [server_target_folder], copied_files_dict=copied_files_dict)
                self._copy_dependency_files(
                    dep, [server_unit_test_target_folder], include_gtest=True, copied_files_dict=copied_files_dict)

            for transitive_dep in dep.dependencies.values():
                if dep_name in self.client_deps:
                    self._copy_dependency_files(
                        transitive_dep, [client_target_folder], copied_files_dict=copied_files_dict)
                    self._copy_dependency_files(transitive_dep, [
                                                client_unit_test_target_folder], include_gtest=True, copied_files_dict=copied_files_dict)

                if dep_name in self.server_deps:
                    self._copy_dependency_files(
                        transitive_dep, [server_target_folder], copied_files_dict=copied_files_dict)
                    self._copy_dependency_files(transitive_dep, [
                                                server_unit_test_target_folder], include_gtest=True, copied_files_dict=copied_files_dict)

        for target_folder, copied_files in copied_files_dict.items():
            print(f"Copied files to {target_folder}: {set(copied_files)}")
