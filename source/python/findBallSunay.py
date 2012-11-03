#!/usr/bin/python

import cv2
import numpy as np
import serial


capture = cv2.VideoCapture(1)
cv2.cv.NamedWindow("track", 0)
capture.set(3, 320)
capture.set(4, 240)
#capture = cv.CaptureFromCAM(0)
#storage = cv.CreateMemStorage(0)

ser1 = serial.Serial('/dev/ttyACM0', 115200) #Right
ser2 = serial.Serial('/dev/ttyACM1', 115200) #Left, negative speeds go forward

#Default trackbar positsion
a = 3
b = 200
c = 245
d = 25
e = 255
f = 255
#initialization of global variables
rel_pos = 0

#making trackbars
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

#Turn off automatic stopping
ser1.write('fs0\n')
ser2.write('fs0\n')

#First IR ball check query
ser1.write('ko\n')
count = 0

def thresholdedImg(capture, h_min, s_min, v_min, h_max, s_max, v_max):
    '''reads an image from the feed and thresholds it using the provided min/max HSV values'''
    ret, img = capture.read() #get the picture to work with
    #img = cv2.flip(img, -1) #flip if necessary
    img = cv2.GaussianBlur(img, (3, 3), 1) #blur
    img_hsv = cv2.cvtColor(img,cv2.COLOR_BGR2HSV) #convert the img to HSV colourspace

    #thresholds for across zero thresholding
    #ORANGE_NOTZERO = np.array([180, e, f],np.uint8)
    #ORANGE_ZERO = np.array([0, b, c],np.uint8)

    thresh_MIN = np.array([h_min, s_min, v_min],np.uint8) #lower threshold
    thresh_MAX = np.array([h_max, s_max, v_max],np.uint8) #higher threshold
    img_thresholded = cv2.inRange(img_hsv, ORANGE_MIN, ORANGE_MAX)

    #img_thresholded_2 = cv2.inRange(img_hsv, ORANGE_ZERO, ORANGE_MAX)
    #img_thresholded = img_thresholded_1 + img_thresholded_2 #adds two ranges
    cv2.imshow('thresholded', img_thresholded) #show picture for calibrating
    return img_thresholded

def findBlobCenter(img_thresholded, minSize):
    '''using a binary image finds the center of the biggest blob and returns it's coordinates as a tuple'''
    contours, hierarchy = cv2.findContours(img_thresholded, cv2.RETR_TREE,cv2.CHAIN_APPROX_TC89_KCOS) #Find contours
    #finds the biggest blob
    bigsize = 0
    biggest = []
    for i in range(len(contours)):
        size = cv2.contourArea(contours[i])
        if size > bigsize and size > minSize:
            bigsize = size
            biggest = contours[i]
    if len(biggest) != 0: #if there is a blob
        M = cv2.moments(biggest)
        centroid_x = int(M['m10']/M['m00']) #for direction
        centroid_y = int(M['m01']/M['m00']) #for distance
        #print(centroid_x, centroid_y) #for debugging
        return (centroid_x, centroid_y)
    else:
        return 0

def drive(centroids, max_spd, slower_by):
    '''Drives towards the provided point (assuming the camera faces forward). Turning rate varies depending on x coordinate.'''
    global rel_pos
    if centroids != 0:
        rel_pos = (centroids[0] - 160)/160.0 #horisontal position of blob in vision -1 left edge, 1 right edge, 0 center
        max_spd = 30 #fastest wheel speed
        slw_spd = max_spd - int(rel_pos*slower_by) #slower wheel speed

        if rel_pos < 0: #blob right of center
            ser1.write('sd'+str(slw_spd)+'\n')
            ser2.write('sd'+str(-max_spd)+'\n')
        elif rel_pos > 0: #blob left of center
            ser1.write('sd'+str(max_spd)+'\n')
            ser2.write('sd'+str(-slw_spd)+'\n')
        else: #blob exactly in the middle
            ser1.write('sd'+str(max_spd)+'\n')
            ser2.write('sd-'+str(max_spd)+'\n')

    else: #no blob in view
        if rel_pos > 0: #if blob was last seen on the right, turn right
            ser1.write('sd25\n')
            ser2.write('sd5\n')
        else: #if blob was last seen on the left, turn left
            ser1.write('sd-5\n')
            ser2.write('sd-25\n')

def ballCheck(ser):
    '''checks if ball is in dribbler
    returns 1 if yes and 0 if no'''
    ko = ser.readLine()
    ser.write('ko\n')
    return int(ko[3])

#main loop
while True:
    #ball check
    ko = ballCheck(ser1)
    if ko == 0: #ball not in dribbler
        count = 0
        img_thresholded = thresholdedImg(capture, a, b, c, d, e, f) #trackbars orange ball
        centroids = findBlobCenter(img_thresholded, 0)
        drive(centroids, 30, 35)

#code for reading feedback, not used for now
        #ser1.write('s\n')
        #ser2.write('s\n')
        #ser1_fb = int(ser1.readline()[3:-2])
        #ser2_fb = int(ser2.readline()[3:-2])
        #if not (ser1_fb == '<s:0>\n' and ser2_fb == '<s:0>\n'):
        #    print(ser1_fb)
        #    print(ser2_fb)

    else: #Ball in dribbler
        img_thresholded = thresholdedImg(capture, 100, 135, 35, 150, 255, 255) #blue goal
        centroids = findBlobCenter(img_thresholded, 1000)
        drive(centroids, 30, 35)
        
     #       print(count)
      #      if 100 < count:
       #         drive_fw = 0
        #        while drive_fw < 10:
         #           ser1.write('sd20\n')
          #          ser2.write('sd-20\n')
           #         drive_fw += 1
    
    if centroids != 0:
        cv2.circle(img, (centroids[0], centroids[1]), 5, (255, 0, 0), -1) #draws a small blue circle at the biggest blob's center for debugging
    cv2.imshow('blurred', img) #show img for calibration

    if cv2.waitKey(10) == 27: #quit on esc
        break
    
#on quit stop the motors
ser1.write('fs1\n')
ser2.write('fs1\n')
ser1.write('sd0\n')
ser2.write('sd0\n')

#clean and release
cv2.destroyAllWindows()
cv2.VideoCapture(0).release()
