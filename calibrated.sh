
v4l2-ctl -d /dev/video1 -c red_balance=128
v4l2-ctl -d /dev/video1 -c blue_balance=128
v4l2-ctl -d /dev/video1 -c exposure=138
v4l2-ctl -d /dev/video1 -c main_gain=0
v4l2-ctl -d /dev/video1 -c sharpness=0
v4l2-ctl -d /dev/video1 -c hflip=1
v4l2-ctl -d /dev/video1 -c vflip=1
python currentcode.py
