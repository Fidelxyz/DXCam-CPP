# DXCam-CPP

A high performance screen capturing library for Windows
porting [DXcam](https://github.com/ra1nty/DXcam) to C++.

[![PyPI - Version](https://img.shields.io/pypi/v/dxcam-cpp)](https://pypi.org/project/dxcam-cpp/)
![Vcpkg Version](https://img.shields.io/vcpkg/v/dxcam-cpp)
[![CI](https://github.com/Fidelxyz/DXCam-CPP/actions/workflows/workflow.yml/badge.svg)](https://github.com/Fidelxyz/DXCam-CPP/actions/workflows/workflow.yml)
[![GitHub License](https://img.shields.io/github/license/Fidelxyz/DXCam-CPP)](https://github.com/Fidelxyz/DXCam-CPP/blob/main/LICENSE)

## Usage

See the [**documentation**](https://dxcam-cpp.readthedocs.io/) for detailed
usage.

### C++ Library

DXCam-CPP is available on [vcpkg](https://github.com/microsoft/vcpkg).

Pre-built binaries are also available
on the [Releases](https://github.com/Fidelxyz/DXCam-CPP/releases) page.

### Python Module

The interface of the Python module of DXCam-CPP is designed to be **fully
compatible** with the original [DXcam](https://github.com/ra1nty/DXcam) as an
alternative.

## Build

### Dependencies

- CMake
- Visual Studio 2022
- [OpenCV](https://github.com/opencv/opencv)

#### For Python Bindings

- [pybind11](https://github.com/pybind/pybind11)

#### For Documentation

- [Doxygen](https://github.com/doxygen/doxygen)
- [Sphinx](https://github.com/sphinx-doc/sphinx)
- [Breathe](https://github.com/breathe-doc/breathe)

### Build for C++ Library

```powershell
cmake -B ./build --preset release
cmake --build ./build
cmake --install ./build --prefix ./build/install
```

### Build for Python Package

```powershell
$env:CMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
pip install .
```
