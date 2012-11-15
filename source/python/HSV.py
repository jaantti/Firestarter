#!/usr/bin/python

import cv
from cv import *
import serial
import time

#ser1 = serial.Serial('/dev/ttyACM1', 115200)
#ser2 = serial.Serial('/dev/ttyACM0', 115200)
cv.NamedWindow("camera", 1)
capture = cv.CaptureFromCAM(1)
cv.NamedWindow("t2", 1)
cv.NamedWindow("t3", 1)
cv.NamedWindow("track", 1)
cv.NamedWindow("filter", 1)
a = 5
b = 125
c = 125
d = 12
e = 255
f = 255
g = 50
h = 200
def track(s):
    global a
    a = s
def track1(s1):
    global b
    b = s1
def track2(s2):
    global c
    c = s2
def track3(s3):
    global d
    d = s3
def track4(s4):
    global e
    e = s4
def track5(s5):
    global f
    f = s5
def thresh(s6):
    global g
    g = s6
def thresh1(s7):
    global h
    h = s7
cv.CreateTrackbar("hmin", "track", a, 180, track)
cv.CreateTrackbar("smin", "track", b, 255, track1)
cv.CreateTrackbar("vmin", "track", c, 255, track2)
cv.CreateTrackbar("hmax", "track", d, 180, track3)
cv.CreateTrackbar("smax", "track", e, 255, track4)
cv.CreateTrackbar("vmax", "track", f, 255, track5)
cv.CreateTrackbar("threshlow", "filter", g, 500, thresh)
cv.CreateTrackbar("threshhigh", "filter", h, 500, thresh1)

while True:
    maxRadius = 0
    img = cv.QueryFrame(capture)
    cv.Smooth(img, img, cv.CV_BLUR, 3)
    edges = cv.CreateImage(cv.GetSize(img), IPL_DEPTH_8U, 1)
    storage = cv.CreateMat(img.width, 1, cv.CV_32FC3)
    hsv = cv.CreateImage(cv.GetSize(img), 8, 3)
    cv.CvtColor(img, hsv, cv.CV_BGR2HSV)
    thresholded_img =  cv.CreateImage(cv.GetSize(hsv), 8, 1)
    cv.InRangeS(hsv, (a, b, c), (d, e, f), thresholded_img)
    cv.Smooth(thresholded_img, thresholded_img, CV_GAUSSIAN, 9, 9)
    cv.Dilate(thresholded_img, thresholded_img)
    Canny(thresholded_img, edges, g, h, 3)
    cv.HoughCircles(edges, storage, cv.CV_HOUGH_GRADIENT, 1, img.height/2, h, h/2, 5, 250)
    value = bool()
    for i in xrange(storage.height - 1):
         if maxRadius < int(storage[i, 0][2]):
             value = True
             x = int(storage[i, 0][0])
             y = int(storage[i, 0][1])
             maxRadius = int(storage[i, 0][2])

    if value:
        print((x, y), maxRadius)
        cv.Circle(img, (x, y), maxRadius, (15, 255, 255), 3, 8, 0)
##        if x < 320:
##                       
##
## 
##         cv.Circle(thresholded_img, center, radius, (15, 255, 255), 3, 8, 0)
##         cv.Circle(edges, center, radius, (0, 0, 255), 3, 8, 0)
    cv.ShowImage("camera", thresholded_img)
    cv.ShowImage("t2", edges)
    cv.ShowImage("t3", img)
    if cv.WaitKey(10) == 27:
        break
cv.DestroyAllWindows()
