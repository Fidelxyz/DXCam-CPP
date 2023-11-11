# DXCam-CPP

A high performance screen capturing library for Windows
rewriting [DXcam](https://github.com/ra1nty/DXcam) in C++.

STILL IN DEVELOPMENT.

## TODO

- [ ] Package as a Python module
- [ ] Unit test (if possible)
- [ ] Benchmark comparing with other libs
- [ ] Github Actions

Contributions are welcome!

## Usage

Include `dxcam.h` in your project:

```cpp
#include "dxcam.h"
```

[OpenCV](https://github.com/opencv/opencv) is also required, since the
screenshot is returned as a `cv::Mat` object.

```cpp
#include "opencv2/opencv.hpp"
```

Everything provided by DXCam_CPP is in the `DXCam` namespace.

Most of the usage is the same as the Python version of DXCam, **but the full
consistency the interface with the Python version is not guaranteed**.

### Initialization

To create a DXCamera instance, you can call `create`, which returns a
shared pointer to the `DXCamera` instance.

```cpp
std::shared_ptr<DXCam::DXCamera> camera = DXCam::create();
```

### Screenshot

```cpp
cv::Mat frame = camera.grab();
```

It is worth nothing that `.grab` will return an empty `cv::Mat` such
that `cv::Mat.empty() == true` if there is no new frame since
the last time you called `.grab`.

To screenshot a specific region, use the `region` parameter: it takes
`DXCam::Region{left, top, right, bottom}` as the left, top, right, bottom
coordinates of the bounding box.

```cpp
cv::Mat frame = camera.grab({0, 0, 1920, 1080});
```

### Screen Capture

To start a screen capture, simply use `.start`: the capture will be started in a
separated thread, default at 60Hz. Use `.stop` to stop the capture.

```cpp
camera.start();
assert(camera.is_capturing == true);
// do something
camera.stop();
assert(camera.is_capturing == false);
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
const std::span<cv::Mat> *frame_buffer;
const int *head, tail;
const size_t *len;
const bool *full;
std::mutex *frame_buffer_all_mutex;

camera.get_frame_buffer(&frame_buffer, &head, &tail, &len, &full, &frame_buffer_all_mutex);

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
manually `delete` it:

```cpp
std::shared_ptr<DXCam::DXCamera> camera1 = DXCam::create(0);
std::shared_ptr<DXCam::DXCamera> camera2 = DXCam::create(0);  // Not allowed, camera1 will be returned
assert(camera1.get() == camera2.get());
delete camera1, camera2;
camera2 = DXCam::create(0);  // Allowed
```

## Benchmarks

> To be completed.

_Data obtained from running on my Laptop (i7-11800H & RTX 3060 Laptop), which a
capturing region of 1920x1080._

### For Max FPS Capability

10000 loops, best of 5: **208466.230555 FPS**

I am not sure if this result is correct (capturing is not optimized away),
although I have no evidence to prove it is wrong. You can see the code for
benchmark in `benchmark/src/max_fps.cpp`. If anyone have an idea about this,
please notify me.

### For Targeting FPS

| Target FPS | mean(FPS), std(Frame Generation Time) |
|------------|---------------------------------------|
| 30         | 30.105708, 0.003644                   |
| 60         | 61.469291, 0.002615                   |
| 90         | 88.202446, 0.002124                   |
| 120        | 119.426397, 0.001867                  |
| 180        | 183.247031, 0.001520                  |
| 240        | 228.854428, 0.001356                  |
| 360        | 413.697012, 0.000989                  |
