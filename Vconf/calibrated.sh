#!/bin/sh

v4l2-ctl -d /dev/video0 -c brightness=48
v4l2-ctl -d /dev/video0 -c contrast=32
v4l2-ctl -d /dev/video0 -c saturation=92
v4l2-ctl -d /dev/video0 -c hue=90
v4l2-ctl -d /dev/video0 -c white_balance_automatic=0
v4l2-ctl -d /dev/video0 -c exposure=200
v4l2-ctl -d /dev/video0 -c gain_automatic=0
v4l2-ctl -d /dev/video0 -c gain=11
./vision_conf


