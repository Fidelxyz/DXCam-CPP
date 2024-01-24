import time

import numpy as np

REPEATS = 100


def benchmark(title: str, dxcam):
    camera = dxcam.create(region=(0, 0, 1920, 1080))
    durations = []
    i = 0
    while i < REPEATS:
        begin_time = time.perf_counter_ns()
        frame = camera.grab()
        end_time = time.perf_counter_ns()
        if frame is not None:
            durations.append((end_time - begin_time) / (10 ** 6))
            i += 1

    durations = np.array(durations)
    mean = np.mean(durations)
    std = np.std(durations)
    print(f"{title}: {mean:.3f} ms/frame ± {std:.3f} ms/frame")


if __name__ == '__main__':
    # import dxcam
    import dxcam_cpp as dxcam

    benchmark("DXCam Benchmark", dxcam)
