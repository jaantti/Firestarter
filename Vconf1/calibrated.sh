#!/bin/sh

vv4l2-ctl -d /dev/video0 -c brightness=48
v4l2-ctl -d /dev/video0 -c contrast=32
v4l2-ctl -d /dev/video0 -c saturation=28
v4l2-ctl -d /dev/video0 -c hue=-14
v4l2-ctl -d /dev/video0 -c white_balance_automatic=0
v4l2-ctl -d /dev/video0 -c exposure=253
v4l2-ctl -d /dev/video0 -c gain_automatic=0
v4l2-ctl -d /dev/video0 -c gain=6
v4l2-ctl -d /dev/video0 -c horizontal_flip=0
v4l2-ctl -d /dev/video0 -c vertical_flip=0
v4l2-ctl -d /dev/video0 -c power_line_frequency=0
v4l2-ctl -d /dev/video0 -c sharpness=0
v4l2-ctl -d /dev/video0 -c auto_exposure=0
v4l2-ctl --set-fmt-video=width=640,height=480,pixelformat=0
v4l2-ctl -p 60

./vision_conf


