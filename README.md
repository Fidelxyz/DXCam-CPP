# DXCam-CPP

A high performance screen capturing library for Windows
rewriting [DXcam](https://github.com/ra1nty/DXcam) in C++.

[Documentation](https://dxcam-cpp.readthedocs.io/)

[![PyPI - Version](https://img.shields.io/pypi/v/dxcam-cpp)](https://pypi.org/project/dxcam-cpp/)
[![PyPI - Downloads](https://img.shields.io/pypi/dm/dxcam-cpp)](https://pypi.org/project/dxcam-cpp/)
[![CI](https://github.com/Fidelxyz/DXCam-CPP/actions/workflows/workflow.yml/badge.svg)](https://github.com/Fidelxyz/DXCam-CPP/actions/workflows/workflow.yml)
[![GitHub License](https://img.shields.io/github/license/Fidelxyz/DXCam-CPP)](https://github.com/Fidelxyz/DXCam-CPP/blob/main/LICENSE)

## Features

- C++ Dynamic Library
- Python Module (Compatible with the original
  Python [DXcam](https://github.com/ra1nty/DXcam) as an alternative)

### Roadmap

- Benchmark comparing with other libs

Contributions are welcome!

## Build

### Dependencies

- CMake
- Visual Studio 2022
- [OpenCV](https://github.com/opencv/opencv)

#### For Documentation

- [Doxygen](https://github.com/doxygen/doxygen)
- [Sphinx](https://github.com/sphinx-doc/sphinx)
- [Breathe](https://github.com/breathe-doc/breathe)

### Build for C++ Library

```powershell
cmake -B ./build -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build ./build --config Release
cmake --install ./build --config Release --prefix ./build/install
```

### Build for Python Package

```powershell
$env:CMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cd python
pip install .
```

## Benchmarks

> Benchmark is not available currently, due to some unresolved issues.

_Data obtained from running on my Laptop (i7-11800H & RTX 3060 Laptop)
with [**UFO Test**](https://www.testufo.com/), which a capturing region of
1920x1080._
