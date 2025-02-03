Reference
#########

Functions
=========

.. doxygenfunction:: DXCam::create(int device_idx = 0, int output_idx = -1, size_t max_buffer_len = 64)

.. doxygenfunction:: DXCam::create(const Region &region, int device_idx = 0, int output_idx = -1, size_t max_buffer_len = 64)

.. doxygenfunction:: DXCam::get_devices_info

.. doxygenfunction:: DXCam::get_outputs_info

Structs
=======

.. doxygenstruct:: DXCam::DeviceInfo
   :members:

.. doxygenstruct:: DXCam::OutputInfo
   :members:

Classes
=======

.. doxygenclass:: DXCam::DXCamera
   :members:

.. doxygenclass:: DXCam::Region
   :members:
