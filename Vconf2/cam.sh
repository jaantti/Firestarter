v4l2-ctl -d /dev/video1 -c brightness=48
v4l2-ctl -d /dev/video1 -c contrast=32
v4l2-ctl -d /dev/video1 -c saturation=28
v4l2-ctl -d /dev/video1 -c hue=-14
v4l2-ctl -d /dev/video1 -c white_balance_automatic=0
v4l2-ctl -d /dev/video1 -c exposure=253
v4l2-ctl -d /dev/video1 -c gain_automatic=0
v4l2-ctl -d /dev/video1 -c gain=6
v4l2-ctl -d /dev/video1 -c horizontal_flip=0
v4l2-ctl -d /dev/video1 -c vertical_flip=0
v4l2-ctl -d /dev/video1 -c power_line_frequency=0
v4l2-ctl -d /dev/video1 -c sharpness=0
v4l2-ctl -d /dev/video1 -c auto_exposure=0
v4l2-ctl --set-fmt-video=width=640,height=480,pixelformat=0
v4l2-ctl -p 60
