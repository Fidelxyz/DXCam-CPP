import cv2

# import dxcam
import dxcam_cpp as dxcam

print(dxcam.device_info())
print(dxcam.output_info())

camera = dxcam.create(region=(0, 0, 1920, 1080), output_color='BGR')  # BGR for OpenCV
image = camera.grab()
cv2.imshow("image", image)
cv2.waitKey()
