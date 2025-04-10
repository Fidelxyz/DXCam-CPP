Basics
######

Installation
============

.. code-block:: shell

   pip install dxcam_cpp

Usage
=====

The interface of the Python module of DXCam_CPP is designed to be **fully
compatible** with the original Python DXcam as an alternative.

Therefore, you can simply import this module and use it as DXcam:

.. code-block:: python

   import dxcam_cpp as dxcam

See `the
documentation of DXcam
<https://github.com/ra1nty/DXcam?tab=readme-ov-file#usage>`_ for more details.

.. note::

Consuming frames from buffer
============================

DXCam_CPP provides access to the frame buffer via the ``frame_buffer()`` method.
With the context manager, you can obtain a list of frames.

.. code-block:: python

   camera = dxcam.create(max_buffer_len=16)

   camera.start()
   with camera.frame_buffer() as buffer:
       # Do something with the buffer
       pass
   camera.stop()
