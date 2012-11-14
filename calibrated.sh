
v4l2-ctl -d /dev/video1 -c red_balance=137
v4l2-ctl -d /dev/video1 -c blue_balance=145
v4l2-ctl -d /dev/video1 -c exposure=255
v4l2-ctl -d /dev/video1 -c main_gain=15
v4l2-ctl -d /dev/video1 -c sharpness=0
python currentcode.py
