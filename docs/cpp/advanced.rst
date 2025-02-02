Advanced Usage and Remarks
##########################

Multiple monitors / GPUs
========================

.. code-block:: cpp

   std::shared_ptr<DXCam::DXCamera> cam1 = DXCam::create(0, 0);
   std::shared_ptr<DXCam::DXCamera> cam2 = DXCam::create(0, 1);
   std::shared_ptr<DXCam::DXCamera> cam3 = DXCam::create(1, 1);
   cv::Mat img1 = cam1->grab();
   cv::Mat img2 = cam2->grab();
   cv::Mat img3 = cam3->grab();

The above code creates three ``DXCamera`` instances
for: ``[monitor0, GPU0], [monitor1, GPU0], [monitor1, GPU1]``, and subsequently
takes three full-screen screenshots. (Like Python DXcam, cross-GPU untested, but
I also hope it works.)

To get a complete list of devices and outputs, use ``get_devices_info``
and ``get_outputs_info``:

.. code-block:: cpp

   std::vector<DXCam::DeviceInfo> devices_info = DXCam::get_devices_info();
   std::vector<std::vector<DXCam::OutputInfo>> outputs_info = DXCam::get_outputs_info();

Here is the declaration of ``DeviceInfo`` and ``OutputInfo``:

.. code-block:: cpp

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

Video Buffer
============

The captured frames will be insert into a fixed-size ring buffer, and when the
buffer is full the newest frame will replace the oldest frame. You can specify
the max buffer length (default to 64) using the argument ``max_buffer_len`` upon
creation of the ``DXCamera`` instance.

.. code-block:: cpp

   DXCam::DXCamera camera = DXCam::create(0, -1, 512);

Consume Frames from Buffer
==========================

To consume frames during capturing there is ``get_latest_frame`` available which
assume the user to process frames in a LIFO pattern. This is a read-only action
and won't pop the processed frame from the buffer.

Going further than the Python DXcam, you can use ``.get_frame_buffer`` to obtain
the whole frame buffer:

.. code-block:: cpp

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

The frame buffer is readonly. **You should lock** ``frame_buffer_all_mutex`` **when
reading the frame buffer**.

Target FPS
==========

To set the target FPS, use the ``target_fps`` upon calling ``.start``.

.. code-block:: cpp

   camera.start(60);

Video Mode
==========

The default behavior of ``.get_latest_frame`` only put newly rendered frame in the
buffer, which suits the usage scenario of a object detection/machine learning
pipeline. However, when recording a video that is not ideal since we aim to get
the frames at a constant framerate: When the ``video_mode`` argument is specified
as ``true`` when calling ``.start`` method of a DXCamera instance, the frame buffer
will be fed at the target fps, using the last frame if there is no new frame
available.

Safely Releasing of Resource
============================

Upon deleting all ``std::shared_ptr<DXCamera>`` object referring to a ``DXCamera``
object, it will stop any active capturing, free the buffer and release the
duplicator and staging resource. Actually, you do not need to manually delete it
since ``std::shared_ptr`` will automatically manage the lifetime of the object it
refers to.

Upon calling ``.stop()``, DXCamera will stop the
active capture and free the frame buffer. If you want to manually recreate a
DXCamera instance on the same output with different parameters, you can also
manually delete it by calling ``.reset()``:

.. code-block:: cpp

   std::shared_ptr<DXCam::DXCamera> camera1 = DXCam::create(0);
   std::shared_ptr<DXCam::DXCamera> camera2 = DXCam::create(0);  // Not allowed, camera1 will be returned
   assert(camera1 == camera2);
   camera1.reset();
   camera2 = DXCam::create(0);  // Allowed