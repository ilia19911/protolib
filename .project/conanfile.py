from conan import ConanFile, tools
from conan.tools.cmake import CMake
import re

class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    requires = ( "gtest/[v1.17.0]", )

    def package_id(self):
        print("PROJECT_PACKAGE_ID")

    def configure(self):
        print("PROJECT_CONFIGURE")

    def tool_requirements(self):
        print("PROJECT_TOOL_REQUIREMENTS")
    def build_requirements(self):
        print("PROJECT_BUILD_REQUIREMENTS")

    def requirements(self):
        print("PROJECT_REQUIREMENTS")


    def generate(self):
        print("PROJECT_GENERATE")

    def build(self):
        print("PROJECT_BUILD")
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package_info(self):
        print("PROJECT_PACKAGE_INFO")
