#!/bin/sh

v4l2-ctl -d /dev/video0 -c brightness=45
v4l2-ctl -d /dev/video0 -c contrast=36
v4l2-ctl -d /dev/video0 -c saturation=41
v4l2-ctl -d /dev/video0 -c hue=-8
v4l2-ctl -d /dev/video0 -c white_balance_automatic=0
v4l2-ctl -d /dev/video0 -c red_balance=103
v4l2-ctl -d /dev/video0 -c blue_balance=187
v4l2-ctl -d /dev/video0 -c gamma=106
v4l2-ctl -d /dev/video0 -c exposure=120
v4l2-ctl -d /dev/video0 -c gain_automatic=0
v4l2-ctl -d /dev/video0 -c gain=0
v4l2-ctl -d /dev/video0 -c horizontal_flip=0
v4l2-ctl -d /dev/video0 -c vertical_flip=0
v4l2-ctl -d /dev/video0 -c power_line_frequency=1
v4l2-ctl -d /dev/video0 -c sharpness=63
v4l2-ctl -d /dev/video0 -c auto_exposure=1
v4l2-ctl -d /dev/video0 --set-fmt-video=width=640,height=480,pixelformat=0
v4l2-ctl -d /dev/video0 -p 60

v4l2-ctl -d /dev/video1 -c brightness=45
v4l2-ctl -d /dev/video1 -c contrast=36
v4l2-ctl -d /dev/video1 -c saturation=41
v4l2-ctl -d /dev/video1 -c hue=-8
v4l2-ctl -d /dev/video1 -c white_balance_automatic=0
v4l2-ctl -d /dev/video1 -c red_balance=103
v4l2-ctl -d /dev/video1 -c blue_balance=187
v4l2-ctl -d /dev/video1 -c gamma=106
v4l2-ctl -d /dev/video1 -c exposure=120
v4l2-ctl -d /dev/video1 -c gain_automatic=0
v4l2-ctl -d /dev/video1 -c gain=0
v4l2-ctl -d /dev/video1 -c horizontal_flip=0
v4l2-ctl -d /dev/video1 -c vertical_flip=0
v4l2-ctl -d /dev/video1 -c power_line_frequency=1
v4l2-ctl -d /dev/video1 -c sharpness=63
v4l2-ctl -d /dev/video1 -c auto_exposure=1
v4l2-ctl -d /dev/video1 --set-fmt-video=width=640,height=480,pixelformat=0
v4l2-ctl -d /dev/video1 -p 60
