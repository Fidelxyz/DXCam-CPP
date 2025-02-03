Basics
######

If you are using CMake, you can use ``find_package`` to include DXCam_CPP in
your project:

.. code-block:: cmake

   find_package(DXCam REQUIRED)
   target_include_directories(... PRIVATE ${DXCam_INCLUDE_DIRS})
   target_link_libraries(... PRIVATE ${DXCam_LIBS})

Include ``dxcam.h`` in your project:

.. code-block:: cpp

   #include <dxcam/dxcam.h>

`OpenCV <https://github.com/opencv/opencv>`_ is automatically included, since
the screenshot is returned as a ``cv::Mat`` object.

Everything provided by DXCam_CPP is in the ``DXCam::`` namespace.

Most of the usage is the same as the original Python DXCam, **but the full
consistency the interface of C++ library with the original Python version is not
guaranteed**.

Initialization
==============

To create a DXCamera instance, you may call :cpp:func:`DXCam::create`, which
returns a shared pointer to the :cpp:class:`DXCam::DXCamera` instance.

.. code-block:: cpp

   std::shared_ptr<DXCam::DXCamera> camera = DXCam::create();

Screenshot a single frame
=========================

.. code-block:: cpp

   cv::Mat frame = camera.grab();

It is worth noting that :cpp:func:`DXCam::DXCamera::grab` will return an empty
``cv::Mat`` such that ``cv::Mat.empty() == true`` if there is no new frame since
the last time you called :cpp:func:`DXCam::DXCamera::grab`.

To screenshot a specific region, set the ``region`` argument which takes
:cpp:class:`DXCam::Region{int left, int top, int right, int bottom} <DXCam::Region>`
as the left, top, right, bottom coordinates of the bounding box.

.. code-block:: cpp

   cv::Mat frame = camera.grab({0, 0, 1920, 1080});

Capturing screen continuously
=============================

To start a screen capture, simply call :cpp:func:`DXCam::DXCamera::start`: the
capture will be started in a separated thread, default at 60Hz. Call
:cpp:func:`DXCam::DXCamera::stop` to stop the capture.

.. code-block:: cpp

   camera.start();
   assert(camera.is_capturing() == true);
   // do something
   camera.stop();
   assert(camera.is_capturing() == false);

While the :cpp:class:`DXCam::DXCamera` instance is in capturing mode, you may
call :cpp:func:`DXCam::DXCamera::get_latest_frame` to get the latest frame in
the frame buffer:

.. code-block:: cpp

   cv::Mat frame = camera.get_latest_frame();

Notice that :cpp:func:`DXCam::DXCamera::get_latest_frame` by default will block
until there is a new frame available since the previous call. To change this
behavior, set the ``video_mode`` argument to ``true``. See the :ref:`video_mode`
section for more details.
