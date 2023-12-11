import cv2

# import dxcam
import dxcam_cpp as dxcam

print(dxcam.device_info())
print(dxcam.output_info())

camera = dxcam.create(region=(0, 0, 1920, 1080), output_color='BGR')  # BGR for OpenCV
print(f'{camera.width=}')
print(f'{camera.height=}')
print(f'{camera.channel_size=}')
print(f'{camera.rotation_angle=}')
print(f'{camera.region=}')
print(f'{camera.max_buffer_len=}')
print(f'{camera.is_capturing=}')

# Grab a single frame
image = camera.grab()
cv2.imshow("Single Frame Grabbing", image)
cv2.waitKey()

# Grab frames continuously
try:
    camera.start()
    while True:
        image = camera.get_latest_frame()
        cv2.imshow("Continuous Frame Grabbing", image)
        cv2.waitKey()
except KeyboardInterrupt:
    pass
finally:
    camera.stop()
