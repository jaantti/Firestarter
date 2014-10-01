#!/bin/sh

v4l2-ctl -d /dev/video1 -c brightness=48
v4l2-ctl -d /dev/video1 -c contrast=32
v4l2-ctl -d /dev/video1 -c saturation=28
v4l2-ctl -d /dev/video1 -c hue=-7
v4l2-ctl -d /dev/video1 -c white_balance_automatic=0
v4l2-ctl -d /dev/video1 -c exposure=89
v4l2-ctl -d /dev/video1 -c gain_automatic=0
v4l2-ctl -d /dev/video1 -c gain=5

./vision_conf


