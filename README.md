# DXCam-CPP

A high performance screen capturing library for Windows
rewriting [DXcam](https://github.com/ra1nty/DXcam) in C++.

[![PyPI - Version](https://img.shields.io/pypi/v/dxcam-cpp)](https://pypi.org/project/dxcam-cpp/)
[![PyPI - Downloads](https://img.shields.io/pypi/dm/dxcam-cpp)](https://pypi.org/project/dxcam-cpp/)
[![CI](https://github.com/Fidelxyz/DXCam-CPP/actions/workflows/workflow.yml/badge.svg)](https://github.com/Fidelxyz/DXCam-CPP/actions/workflows/workflow.yml)
[![GitHub License](https://img.shields.io/github/license/Fidelxyz/DXCam-CPP)](https://github.com/Fidelxyz/DXCam-CPP/blob/main/LICENSE)

## Features

- C++ Dynamic Library
- Python Module (Compatible with the original
  Python [DXcam](https://github.com/ra1nty/DXcam) as an alternative)

### TODO

- Benchmark comparing with other libs
- Formal documentation (Doxygen)

Contributions are welcome!

## Usage as a Python Module

```shell
pip install dxcam_cpp
```

The interface of the Python module of DXCam_CPP is designed to be **fully
compatible** with the original Python DXcam as an alternative.
See [the documentation of DXcam](https://github.com/ra1nty/DXcam?tab=readme-ov-file#usage)
for more details.

> Working in progress

## Usage as a C++ Library

If you are using CMake, you can use `find_package` to include DXCam_CPP in your
project:

```cmake
find_package(DXCam REQUIRED)
target_include_directories(... PRIVATE ${DXCam_INCLUDE_DIRS})
target_link_libraries(... PRIVATE ${DXCam_LIBS})
```

Include `dxcam.h` in your project:

```cpp
#include <dxcam/dxcam.h>
```

[OpenCV](https://github.com/opencv/opencv) is automatically included, since the
screenshot is returned as a `cv::Mat` object.

Everything provided by DXCam_CPP is in the `DXCam` namespace.

Most of the usage is the same as the original Python DXCam, **but the full
consistency the interface of C++ library with the original Python version is not
guaranteed**.

### Initialization

To create a DXCamera instance, you can call `create`, which returns a shared
pointer to the `DXCamera` instance.

```cpp
std::shared_ptr<DXCam::DXCamera> camera = DXCam::create();
```

### Screenshot

```cpp
cv::Mat frame = camera.grab();
```

It is worth noting that `.grab` will return an empty `cv::Mat` such
that `cv::Mat.empty() == true` if there is no new frame since
the last time you called `.grab`.

To screenshot a specific region, use the `region` argument: it takes
`DXCam::Region{int left, int top, int right, int bottom}` as the left, top,
right, bottom coordinates of the bounding box.

```cpp
cv::Mat frame = camera.grab({0, 0, 1920, 1080});
```

### Screen Capture

To start a screen capture, simply call `.start`: the capture will be started in
a separated thread, default at 60Hz. Call `.stop` to stop the capture.

```cpp
camera.start();
assert(camera.is_capturing() == true);
// do something
camera.stop();
assert(camera.is_capturing() == false);
```

### Consume the Screen Capture Data

While the `DXCamera` instance is in capture mode, you can
use `.get_latest_frame` to get the latest frame in the frame buffer:

```cpp
cv::Mat frame = camera.get_latest_frame();
```

Notice that `.get_latest_frame` by default will block until there is a new frame
available since the last call to `.get_latest_frame`. To change this behavior,
set the `video_mode` argument to `true`.

## Advanced Usage and Remarks

### Multiple monitors / GPUs

```cpp
std::shared_ptr<DXCam::DXCamera> cam1 = DXCam::create(0, 0);
std::shared_ptr<DXCam::DXCamera> cam2 = DXCam::create(0, 1);
std::shared_ptr<DXCam::DXCamera> cam3 = DXCam::create(1, 1);
cv::Mat img1 = cam1->grab();
cv::Mat img2 = cam2->grab();
cv::Mat img3 = cam3->grab();
```

The above code creates three `DXCamera` instances
for: `[monitor0, GPU0], [monitor1, GPU0], [monitor1, GPU1]`, and subsequently
takes three full-screen screenshots. (Like Python DXcam, cross-GPU untested, but
I also hope it works.)

To get a complete list of devices and outputs, use `get_devices_info`
and `get_outputs_info`:

```cpp
std::vector<DXCam::DeviceInfo> devices_info = DXCam::get_devices_info();
std::vector<std::vector<DXCam::OutputInfo>> outputs_info = DXCam::get_outputs_info();
```

Here is the declaration of `DeviceInfo` and `OutputInfo`:

```cpp
struct DXCam::DeviceInfo {
    std::wstring description;
    SIZE_T vram_size;
    UINT vendor_id;
};

struct DXCam::OutputInfo {
    std::wstring device_name;
    LONG width;
    LONG height;
    int rotation_angle;
};
```

### Video Buffer

The captured frames will be insert into a fixed-size ring buffer, and when the
buffer is full the newest frame will replace the oldest frame. You can specify
the max buffer length (default to 64) using the argument `max_buffer_len` upon
creation of the `DXCamera` instance.

```cpp
DXCam::DXCamera camera = DXCam::create(0, -1, 512);
```

### Consume Frames from Buffer

To consume frames during capturing there is `get_latest_frame` available which
assume the user to process frames in a LIFO pattern. This is a read-only action
and won't pop the processed frame from the buffer.

Going further than the Python DXcam, you can use `.get_frame_buffer` to obtain
the whole frame buffer:

```cpp
const cv::Mat *const *frame_buffer;
const std::atomic_int *head, *tail;
const size_t *len;
const std::atomic_bool *full;
std::mutex *const *frame_buffer_mutex;
std::mutex *frame_buffer_all_mutex;

camera.get_frame_buffer(&frame_buffer, &frame_buffer_mutex, &len, &head,
                        &tail, &full, &frame_buffer_all_mutex);

{
    // you should lock frame_buffer_all_mutex when reading the frame buffer
    std::scoped_lock lock(*frame_buffer_all_mutex);
    
    // read the frame buffer in a correct order
    for (size_t i = 0; i < *len; i++) {
        cv::Mat frame = (*frame_buffer)[(i + *head) % *len];
        // Do something with the frame
    }
}
```

The frame buffer is readonly. **You should lock `frame_buffer_all_mutex` when
reading the frame buffer**.

### Target FPS

To set the target FPS, use the `target_fps` upon calling `.start`.

```cpp
camera.start(60);
```

### Video Mode

The default behavior of `.get_latest_frame` only put newly rendered frame in the
buffer, which suits the usage scenario of a object detection/machine learning
pipeline. However, when recording a video that is not ideal since we aim to get
the frames at a constant framerate: When the `video_mode` argument is specified
as `true` when calling `.start` method of a DXCamera instance, the frame buffer
will be fed at the target fps, using the last frame if there is no new frame
available.

### Safely Releasing of Resource

Upon deleting all `std::shared_ptr<DXCamera>` object referring to a `DXCamera`
object, it will stop any active capturing, free the buffer and release the
duplicator and staging resource. Actually, you do not need to manually delete it
since `std::shared_ptr` will automatically manage the lifetime of the object it
refers to.

Upon calling `.stop()`, DXCamera will stop the
active capture and free the frame buffer. If you want to manually recreate a
DXCamera instance on the same output with different parameters, you can also
manually delete it by calling `.reset()`:

```cpp
std::shared_ptr<DXCam::DXCamera> camera1 = DXCam::create(0);
std::shared_ptr<DXCam::DXCamera> camera2 = DXCam::create(0);  // Not allowed, camera1 will be returned
assert(camera1 == camera2);
camera1.reset();
camera2 = DXCam::create(0);  // Allowed
```

## Build

### Prerequisites

- CMake
- Visual Studio 2022
- [OpenCV](https://github.com/opencv/opencv)

### Build for C++ Library

```shell
cmake -B ./build
cmake --build ./build --config Release
cmake --install ./build --config Release --prefix ./build/install
```

### Build for Python Package

```shell
cd python
pip install .
```

## Benchmarks

> Benchmark is not available currently, due to some unresolved issues.

_Data obtained from running on my Laptop (i7-11800H & RTX 3060 Laptop)
with [**UFO Test**](https://www.testufo.com/), which a capturing region of
1920x1080._