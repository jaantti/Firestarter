v4l2-ctl -d /dev/video1 -c brightness=0
v4l2-ctl -d /dev/video1 -c contrast=32
v4l2-ctl -d /dev/video1 -c saturation=64
v4l2-ctl -d /dev/video1 -c hue=0
v4l2-ctl -d /dev/video1 -c white_balance_automatic=1
v4l2-ctl -d /dev/video1 -c exposure=190
v4l2-ctl -d /dev/video1 -c gain_automatic=1
v4l2-ctl -d /dev/video1 -c gain=4
v4l2-ctl -d /dev/video1 -c horizontal_flip=0
v4l2-ctl -d /dev/video1 -c vertical_flip=0
v4l2-ctl -d /dev/video1 -c power_line_frequency=0
v4l2-ctl -d /dev/video1 -c sharpness=0
v4l2-ctl -d /dev/video1 -c auto_exposure=0
v4l2-ctl --set-fmt-video=width=640,height=480,pixelformat=0
v4l2-ctl -p 30
