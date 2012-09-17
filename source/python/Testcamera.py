#!/usr/bin/python

import cv
from cv import *

cv.NamedWindow("camera", 1)
capture = cv.CaptureFromCAM(0)

while True:
    img = cv.QueryFrame(capture)
    cv.Smooth(img, img, cv.CV_BLUR, 3)

    hsv = cv.CreateImage(cv.GetSize(img), 8, 3)
    cv.CvtColor(img, hsv, cv.CV_BGR2HSV)

    thresholded_img =  cv.CreateImage(cv.GetSize(hsv), 8, 1) 
    cv.InRangeS(hsv, (100, 80, 80), (120, 255, 255), thresholded_img)

    cv.ShowImage("camera", thresholded_img)
    if cv.WaitKey(10) == 27:
        break
cv.DestroyWindow("camera")
