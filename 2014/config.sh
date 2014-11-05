#!/bin/sh

v4l2-ctl -d /dev/video0 -c brightness=48
v4l2-ctl -d /dev/video0 -c contrast=32
v4l2-ctl -d /dev/video0 -c saturation=28
v4l2-ctl -d /dev/video0 -c hue=-7
v4l2-ctl -d /dev/video0 -c white_balance_automatic=0
v4l2-ctl -d /dev/video0 -c exposure=89
v4l2-ctl -d /dev/video0 -c gain_automatic=0
v4l2-ctl -d /dev/video0 -c gain=5

v4l2-ctl -d /dev/video0 --set-fmt-video=width=640,height=480,pixelformat=0
v4l2-ctl -d /dev/video0 -p 60

v4l2-ctl -d /dev/video1 -c brightness=48
v4l2-ctl -d /dev/video1 -c contrast=32
v4l2-ctl -d /dev/video1 -c saturation=28
v4l2-ctl -d /dev/video1 -c hue=-7
v4l2-ctl -d /dev/video1 -c white_balance_automatic=0
v4l2-ctl -d /dev/video1 -c exposure=89
v4l2-ctl -d /dev/video1 -c gain_automatic=0
v4l2-ctl -d /dev/video1 -c gain=5

v4l2-ctl -d /dev/video1 --set-fmt-video=width=640,height=480,pixelformat=0
v4l2-ctl -d /dev/video1 -p 60
