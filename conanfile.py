import os
from conans import ConanFile, CMake
class TradingSystems(ConanFile):
    name = "Trade"
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}

    def build(self):
        # build your shared library
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
    def package(self):
        # Copy the executable
        #self.copy(pattern="Trade*", dst="build", keep_path=False)
        # Copy the libraries
        if self.options.shared:
            self.copy(pattern="*.dll", dst="bin", keep_path=False)
            self.copy(pattern="*.dylib", dst="lib", keep_path=False)
            self.copy(pattern="*.so*", dst="lib", keep_path=False)