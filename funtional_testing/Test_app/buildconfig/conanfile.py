from conans import ConanFile, CMake

class TestAppConan(ConanFile):
    name = "TestApp"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    requires = "gtest/1.14.0"
    default_options = {"gtest:shared": False}

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()