from conans import ConanFile, CMake, tools
import os
import shutil


class DiagnosticManagerConan(ConanFile):
    name = "diagnostic-manager"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    exports_sources = "../dev/*", "CMakeLists.txt"
    # diagnostic-manager no longer requires ara-diag for local builds
    # requires = "ara-diag/0.1@"

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".")
        cmake.build()

    def source(self):
        """Copy the local ../dev folder into the Conan source folder so CMake can find it.

        Ensures a consistent layout when `conan create` runs in an isolated cache build folder.
        """
        recipe_dev = os.path.normpath(os.path.join(self.recipe_folder, "..", "dev"))
        dest_dev = os.path.join(self.source_folder, "dev")
        if os.path.isdir(recipe_dev):
            if os.path.exists(dest_dev):
                shutil.rmtree(dest_dev)
            shutil.copytree(recipe_dev, dest_dev)

    def export_sources(self):
        recipe_dev = os.path.normpath(os.path.join(self.recipe_folder, "..", "dev"))
        if os.path.isdir(recipe_dev):
            dest = os.path.join(self.export_sources_folder, "dev")
            if os.path.exists(dest):
                shutil.rmtree(dest)
            shutil.copytree(recipe_dev, dest)

    def package(self):
        self.copy("*.h", dst="include", src="../dev/inc")
        self.copy("*.a", dst="lib", keep_path=False)
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.so*", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["diagnostic-manager"]
