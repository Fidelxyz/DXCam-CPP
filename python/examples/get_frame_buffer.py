from time import sleep

import dxcam_cpp as dxcam

if __name__ == '__main__':
    print(dxcam.device_info())
    print(dxcam.output_info())

    camera = dxcam.create(region=(0, 0, 1920, 1080),
                          output_color='BGR',  # BGR for OpenCV
                          max_buffer_len=16)
    print(f'{camera.width=}')
    print(f'{camera.height=}')
    print(f'{camera.channel_size=}')
    print(f'{camera.rotation_angle=}')
    print(f'{camera.region=}')
    print(f'{camera.max_buffer_len=}')
    print(f'{camera.is_capturing=}')

    camera.start()

    sleep(1)

    with camera.frame_buffer() as buffer:
        print("Buffer Length:", len(buffer))
        print(buffer)

    camera.stop()
