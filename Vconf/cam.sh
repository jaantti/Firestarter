v4l2-ctl -d /dev/video1 -c brightness=0
v4l2-ctl -d /dev/video1 -c contrast=27
v4l2-ctl -d /dev/video1 -c saturation=48
v4l2-ctl -d /dev/video1 -c hue=-12
v4l2-ctl -d /dev/video1 -c auto_white_balance=0
v4l2-ctl -d /dev/video1 -c exposure=205
v4l2-ctl -d /dev/video1 -c auto_gain=0
v4l2-ctl -d /dev/video1 -c main_gain=17
v4l2-ctl -d /dev/video1 -c hflip=0
v4l2-ctl -d /dev/video1 -c vflip=1
v4l2-ctl -d /dev/video1 -c light_frequency_filter=1
v4l2-ctl -d /dev/video1 -c sharpness=0
v4l2-ctl -d /dev/video1 -c auto_exposure=0
v4l2-ctl --set-fmt-video=width=640,height=480,pixelformat=0
v4l2-ctl -p 60
