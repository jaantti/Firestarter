#!/usr/bin/python

import cv2
import numpy as np
import serial
 
capture = cv2.VideoCapture(1)
cv2.cv.NamedWindow("track", 0)
#capture = cv.CaptureFromCAM(0)
#storage = cv.CreateMemStorage(0)

ser1 = serial.Serial('/dev/ttyACM0', 115200)
ser2 = serial.Serial('/dev/ttyACM1', 115200)
 
a = 5
b = 125
c = 125
d = 12
e = 255
f = 255
 
def tracka(s):
    global a
    a = s
cv2.cv.CreateTrackbar("hmin", "track", a, 180, tracka)
 
def trackb(s):
    global b
    b = s    
cv2.cv.CreateTrackbar("smin", "track", b, 255, trackb)
 
def trackc(s):
    global c
    c = s
cv2.cv.CreateTrackbar("vmin", "track", c, 255, trackc)
 
def trackd(s):
    global d
    d = s
cv2.cv.CreateTrackbar("hmax", "track", d, 180, trackd)
 
def tracke(s):
    global e
    e = s
cv2.cv.CreateTrackbar("smax", "track", e, 255, tracke)
 
def trackf(s):
    global f
    f = s
cv2.cv.CreateTrackbar("vmax", "track", f, 255, trackf)
 
 
while True:
    ret, img = capture.read()
    #img = cv2.medianBlur(img, 51)
    #img = cv2.erode(img, np.ones((11,11),'int'))
   
    img_hsv = cv2.cvtColor(img,cv2.COLOR_BGR2HSV)
 
    ORANGE_MIN = np.array([a, b, c],np.uint8)
    ORANGE_MAX = np.array([d, e, f],np.uint8)
    img_thresholded = cv2.inRange(img_hsv, ORANGE_MIN, ORANGE_MAX)
    # ball orange (5, 125, 125), (12, 255, 255)
    # pen blue (100, 80, 80), (120, 255, 255)
    cv2.imshow('thresholded', img_thresholded)
 
    contours, hierarchy = cv2.findContours(img_thresholded, cv2.RETR_TREE,cv2.CHAIN_APPROX_TC89_KCOS)
 
    bigsize = 0
    biggest = [] #np.ones((2,2),'int')
    for i in range(len(contours)):
        size = cv2.contourArea(contours[i])
        if size > bigsize:
            bigsize = size
            biggest = contours[i]
 
    if len(biggest) != 0:
        #approx = cv2.approxPolyDP(contours[i], 0.02*cv2.arcLength(contours[i],True),True)
        #hull = cv2.convexHull(contours[i])
        #cv2.drawContours(img, biggest, 0, (0, 0, 255), -1)
        M = cv2.moments(biggest)
        centroid_x = int(M['m10']/M['m00'])
        centroid_y = int(M['m01']/M['m00'])
        print(centroid_x, centroid_y)
        if centroid_x > 340:
            #drive_right
            print('Wrumm! Going right.')
			ser1.write('sd20\n')
			ser2.write('sd20\n')
        elif centroid_x < 300:
            print('Wrumm! Going left.')
			ser1.write('sd-20\n')
			ser2.write('sd-20\n')
        else:
            print('Wrumm! Full speed ahead.')
 
 
    cv2.imshow('blurred', img)
   
    if cv2.waitKey(10) == 27:
        break
 
cv2.destroyAllWindows()
cv2.VideoCapture(0).release()
##cv.DestroyWindow("camera2")
