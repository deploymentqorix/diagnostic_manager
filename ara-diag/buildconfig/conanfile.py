from conans import ConanFile, CMake, tools
import os
import shutil


class AraDiagConan(ConanFile):
    name = "ara-diag"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    # Export the dev folder recursively so sources and headers are included
    exports_sources = "../dev/**", "CMakeLists.txt"
    # ara-diag is independent and should not require diagnostic-manager

    def build(self):
        cmake = CMake(self)
        # Compute the absolute dev path in the recipe folder and pass it to CMake
        recipe_dev = os.path.normpath(os.path.join(self.recipe_folder, "..", "dev"))
        defs = {}
        if os.path.isdir(recipe_dev):
            defs['ARA_DEV_ROOT'] = recipe_dev
        cmake.configure(source_folder=".", defs=defs)
        cmake.build()

    def source(self):
        """Copy the local ../dev folder into the Conan source folder so CMake can find it.

        This ensures `conan create` (which runs in a temporary build area) has the
        project's sources available under the expected `dev/` layout.
        """
        recipe_dev = os.path.normpath(os.path.join(self.recipe_folder, "..", "dev"))
        dest_dev = os.path.join(self.source_folder, "dev")
        if os.path.isdir(recipe_dev):
            # Remove any existing dest and copy the tree
            if os.path.exists(dest_dev):
                shutil.rmtree(dest_dev)
            shutil.copytree(recipe_dev, dest_dev)

    def export_sources(self):
        """Ensure the `dev` folder is exported with the recipe into the Conan cache.

        Some conan workflows copy the recipe to a cache location before build; this
        guarantees that `dev/` is included in the exported sources that end up in
        the isolated build `source_folder`.
        """
        recipe_dev = os.path.normpath(os.path.join(self.recipe_folder, "..", "dev"))
        if os.path.isdir(recipe_dev):
            dest = os.path.join(self.export_sources_folder, "dev")
            if os.path.exists(dest):
                shutil.rmtree(dest)
            shutil.copytree(recipe_dev, dest)

    def package(self):
        self.copy("*.h", dst="include", src="../dev/inc/public")
        self.copy("*.so*", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["ara-diag"]
