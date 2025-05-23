"""
Based on https://github.com/pybind/cmake_example/blob/master/setup.py
"""
import os
import subprocess
import sys
from pathlib import Path

from mypy import stubgen
from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext

# Convert distutils Windows platform specifiers to CMake -A arguments
PLAT_TO_CMAKE = {
    "win32": "Win32",
    "win-amd64": "x64",
    "win-arm32": "ARM",
    "win-arm64": "ARM64",
}


# A CMakeExtension needs a sourcedir instead of a file list.
# The name must be the _single_ output extension from the CMake build.
# If you need multiple extensions, see scikit-build.
class CMakeExtension(Extension):
    def __init__(self, name: str, sourcedir: str = "") -> None:
        super().__init__(name, sources=[])
        self.sourcedir = os.fspath(Path(sourcedir).resolve())


class CMakeBuild(build_ext):
    def build_extension(self, ext: CMakeExtension) -> None:
        print()
        print("Setup build...")

        # Must be in this form due to bug in .resolve() only fixed in Python 3.10+
        ext_fullpath = Path.cwd() / self.get_ext_fullpath(ext.name)
        extdir = ext_fullpath.parent.resolve()

        # Using this requires trailing slash for auto-detection & inclusion of
        # auxiliary "native" libs

        debug = int(
            os.environ.get("DEBUG", 0)) if self.debug is None else self.debug
        cfg = "Debug" if debug else "Release"

        # CMake lets you override the generator - we need to check this.
        # Can be set with Conda-Build, for example.
        cmake_generator = os.environ.get("CMAKE_GENERATOR", "")

        # Set Python_EXECUTABLE instead if you use PYBIND11_FINDPYTHON
        # EXAMPLE_VERSION_INFO shows you how to pass a value into the C++ code
        # from Python.
        cmake_args = [
            f"-DPYTHON_EXECUTABLE={sys.executable}",
            f"-DCMAKE_BUILD_TYPE={cfg}",  # not used on MSVC, but no harm
            f"-DBUILD_PYTHON=ON",
            f"-DINSTALL_PYTHON_ONLY=ON",
        ]
        build_args = [
            "--target", ext.name,
        ]
        install_args = [
            "--prefix", str(extdir),
        ]

        # Adding CMake arguments set as environment variable
        # (needed e.g. to build for ARM OSx on conda-forge)
        if "CMAKE_ARGS" in os.environ:
            cmake_args += [item for item in os.environ["CMAKE_ARGS"].split(" ")
                           if item]

        # In this example, we pass in the version to C++. You might not need to.
        cmake_args += [
            f"-DDXCAM_VERSION_INFO={self.distribution.get_version()}"]

        # Single config generators are handled "normally"
        single_config = any(x in cmake_generator for x in {"NMake", "Ninja"})

        # CMake allows an arch-in-generator style for backward compatibility
        contains_arch = any(x in cmake_generator for x in {"ARM", "Win64"})

        # Specify the arch if using MSVC generator, but only if it doesn't
        # contain a backward-compatibility arch spec already in the
        # generator name.
        if not single_config and not contains_arch:
            cmake_args += ["-A", PLAT_TO_CMAKE[self.plat_name]]

        # Multi-config generators have a different way to specify configs
        if not single_config:
            cmake_args += []
            build_args += ["--config", cfg]
            install_args += ["--config", cfg]

        # Set CMAKE_BUILD_PARALLEL_LEVEL to control the parallel build level
        # across all generators.
        if "CMAKE_BUILD_PARALLEL_LEVEL" not in os.environ:
            # self.parallel is a Python 3 only way to set parallel jobs by hand
            # using -j in the build_ext call, not supported by pip or PyPA-build.
            if hasattr(self, "parallel") and self.parallel:
                # CMake 3.12+ only.
                build_args += [f"-j{self.parallel}"]

        build_temp = Path(self.build_temp) / ext.name
        if not build_temp.exists():
            build_temp.mkdir(parents=True)

        print("CMake args:", cmake_args)
        print("Build args:", build_args)
        print("Install args:", install_args)
        print()

        print("Configuring CMake...")
        subprocess.run(
            ["cmake", ext.sourcedir, *cmake_args], cwd=build_temp, check=True
        )
        print()

        print("Building...")
        subprocess.run(
            ["cmake", "--build", ".", *build_args], cwd=build_temp, check=True
        )
        print()

        print("Installing...")
        subprocess.run(
            ["cmake", "--install", ".", *install_args], cwd=build_temp,
            check=True
        )
        print()

        # Generate stubs
        sys.path.insert(0, str(extdir / ext.name))
        stubgen.main(["-m", "dxcam_cpp", "-o", str(extdir / ext.name)])


# The information here can also be placed in setup.cfg - better separation of
# logic and declaration, and simpler if you include description/version in a file.
setup(
    ext_modules=[CMakeExtension("dxcam_cpp")],
    cmdclass={"build_ext": CMakeBuild},
)
