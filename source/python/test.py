#!/usr/bin/python

from SimpleCV import *
import time

cam = Camera()
img = Image('logo')
blobs = img.findBlobs()
green = cam.getImage().colorDistance(Color.GREEN).invert().save('green.png')


print blobs
