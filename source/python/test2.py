#!/usr/bin/python

from SimpleCV import *

cam = Camera()
display = Display()
# This variable saves the last rotation, and is used
# in the while loop to increment the rotation
rotate = 0
while display.isNotDone():
        rotate = rotate + 5
        cam.getImage().rotate(rotate).show()

