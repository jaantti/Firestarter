#!/usr/bin/python

import cv2
import numpy as np
import serial
from time import *


capture = cv2.VideoCapture(1)
cv2.cv.NamedWindow("track", 0)
capture.set(3, 320)
capture.set(4, 240)

ser1 = serial.Serial('/dev/ttyACM0', 115200) #Right
ser1.write('?\n')
sleep(0.5)
if ser1.readline()[4] == 0:
    ser2 = serial.Serial('/dev/ttyACM1', 115200) #Left, negative speeds go forward
else:
    ser1 = serial.Serial('/dev/ttyACM1', 115200)
    ser2 = serial.Serial('/dev/ttyACM0', 115200)

#initialization of global variables
rel_pos = 0
count = 0

#making trackbars
def null(*arg):
    pass


cv2.cv.CreateTrackbar("hmin", "track", 3, 180, null)  
cv2.cv.CreateTrackbar("smin", "track", 200, 255, null)
cv2.cv.CreateTrackbar("vmin", "track", 245, 255, null)
cv2.cv.CreateTrackbar("hmax", "track", 25, 180, null)
cv2.cv.CreateTrackbar("smax", "track", 255, 255, null)
cv2.cv.CreateTrackbar("vmax", "track", 255, 255, null)

def thresholdedImg(img, h_min, s_min, v_min, h_max, s_max, v_max):
    '''reads an image from the feed and thresholds it using the provided min/max HSV values'''
    img = cv2.GaussianBlur(img, (3, 3), 1) #blur
    img_hsv = cv2.cvtColor(img,cv2.COLOR_BGR2HSV) #convert the img to HSV colourspace

    #thresholds for across zero thresholding
    #ORANGE_NOTZERO = np.array([180, e, f],np.uint8)
    #ORANGE_ZERO = np.array([0, b, c],np.uint8)

    thresh_MIN = np.array([h_min, s_min, v_min],np.uint8) #lower threshold
    thresh_MAX = np.array([h_max, s_max, v_max],np.uint8) #higher threshold
    img_thresholded = cv2.inRange(img_hsv, thresh_MIN, thresh_MAX)

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

def drive(centroids, max_spd, slower_by, ser1, ser2):
    '''Drives towards the provided point (assuming the camera faces forward). Turning rate varies depending on x coordinate.'''
    global rel_pos
    global count
    if centroids != 0:
        count = 0
        rel_pos = (centroids[0] - 160)/160.0 #horisontal position of blob in vision -1 left edge, 1 right edge, 0 center
        max_spd = 30 #fastest wheel speed

        if rel_pos < 0: #blob right of center
            ser1.write('sd'+str(max_spd - int(rel_pos*slower_by))+'\n') #slower wheel speed
            ser2.write('sd'+str(-max_spd)+'\n')
        elif rel_pos > 0: #blob left of center
            ser1.write('sd'+str(max_spd)+'\n')
            ser2.write('sd'+str(-max_spd - int(rel_pos*slower_by))+'\n') #slower wheel speed
        else: #blob exactly in the middle
            ser1.write('sd'+str(max_spd)+'\n')
            ser2.write('sd-'+str(max_spd)+'\n')

    else: #no blob in view
        if count < 500: #it hasn't looked long enough
            if rel_pos > 0: #if blob was last seen on the right, turn right
                ser1.write('sd25\n')
                ser2.write('sd5\n')
            else: #if blob was last seen on the left, turn left
                ser1.write('sd-5\n')
                ser2.write('sd-25\n')
            count += 1
        else: #looked long enough, go forward
            ser1.write('sd25\n')
            ser1.write('sd25\n')
            sleep(2)
            count = 0

def ballCheck(ser):
    '''checks if ball is in dribbler
    returns 1 if yes and 0 if no'''
    ko = ser.readline()
    ser.write('ko\n')
    return int(ko[3])


def lineDetection():
    global img
    thresh = cv2.inRange(img, np.array([0, 0, 0],np.uint8), np.array([50, 50, 50],np.uint8))
    cv2.imshow('thresh', thresh)
    edge = cv2.Canny(thresh, 40, 120)
    lines = cv2.HoughLinesP(edge, 1, np.pi/180, 50, minLineLength = 50, maxLineGap = 15)
    lines = np.array(lines, ndmin = 2)
    lines=lines[0]
    for l in lines:
        if(l != None):
            #print(l)
            cv2.line(img, (l[0], l[1]), (l[2],l[3]), (0, 255, 255), 1)

#Turn off automatic stopping
ser1.write('fs0\n')
ser2.write('fs0\n')

#First IR ball check query
ser1.write('ko\n')

#main loop
while True:

    a = cv2.getTrackbarPos('hmin', 'track')
    b = cv2.getTrackbarPos('smin', 'track')
    c = cv2.getTrackbarPos('vmin', 'track')
    d = cv2.getTrackbarPos('hmax', 'track')
    e = cv2.getTrackbarPos('smax', 'track')
    f = cv2.getTrackbarPos('vmax', 'track')

    ret, img = capture.read() #get the picture to work with
    img = cv2.flip(img, -1) #flip if necessary
    ko = ballCheck(ser1) #ball check
    ko = 0
    lineDetection()    

    if ko == 0: #ball not in dribbler
        img_thresholded = thresholdedImg(img, a, b, c, d, e, f) #trackbars orange ball
        centroids = findBlobCenter(img_thresholded, 0)
        drive(centroids, 30, 35, ser1, ser2)

#code for reading feedback, not used for now
        #ser1.write('s\n')
        #ser2.write('s\n')
        #ser1_fb = int(ser1.readline()[3:-2])
        #ser2_fb = int(ser2.readline()[3:-2])
        #if not (ser1_fb == '<s:0>\n' and ser2_fb == '<s:0>\n'):
        #    print(ser1_fb)
        #    print(ser2_fb)

    else: #Ball in dribbler
        img_thresholded = thresholdedImg(img, 100, 135, 35, 150, 255, 255) #blue goal
        centroids = findBlobCenter(img_thresholded, 1000)
        drive(centroids, 30, 35, ser1, ser2)
        
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
