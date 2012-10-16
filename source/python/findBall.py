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
ser2 = serial.Serial('/dev/ttyACM1', 115200) #Left

a = 100
b = 135
c = 35
d = 150
e = 255
f = 255
rel_pos = 0

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


ser1.write('fs0\n')
ser2.write('fs0\n')

ser1.write('ko\n')
count = 0

while True:
    ko = ser1.readline()
    print(ko)
    ser1.write('ko\n')
    if int(ko[3]) == 0:
        count = 0
        
        ret, img = capture.read()
    #    img = cv2.flip(img, -1)
        img = cv2.GaussianBlur(img, (3, 3), 1)
    #    img = cv2.medianBlur(img, 31)
        #img = cv2.erode(img, np.ones((11,11),'int'))
       
        img_hsv = cv2.cvtColor(img,cv2.COLOR_BGR2HSV)

        ORANGE_MIN = np.array([3, 200, 245],np.uint8)
        #ORANGE_NOTZERO = np.array([180, e, f],np.uint8)
        #ORANGE_ZERO = np.array([0, b, c],np.uint8)
        ORANGE_MAX = np.array([25, 255, 255],np.uint8)

        img_thresholded = cv2.inRange(img_hsv, ORANGE_MIN, ORANGE_MAX)
        #img_thresholded_2 = cv2.inRange(img_hsv, ORANGE_ZERO, ORANGE_MAX)
        # ball orange (5, 125, 125), (12, 255, 255)
        # pen blue (100, 80, 80), (120, 255, 255)

        #img_thresholded = img_thresholded_1 + img_thresholded_2
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
            print(bigsize)
            #approx = cv2.approxPolyDP(contours[i], 0.02*cv2.arcLength(contours[i],True),True)
            #hull = cv2.convexHull(contours[i])
            #cv2.drawContours(img, biggest, 0, (0, 0, 255), -1)
            M = cv2.moments(biggest)

            centroid_x = int(M['m10']/M['m00']) #for direction
            centroid_y = int(M['m01']/M['m00']) #for distance
            #print(centroid_x, centroid_y)
            cv2.circle(img, (centroid_x, centroid_y), 5, (255, 0, 0), -1)
            rel_pos = (centroid_x - 160)/160.0
            max_spd = 30
            slw_spd = int(rel_pos*25)
            mtr1_spd = 0
            mtr2_spd = 0

            if rel_pos < 0:

                ser1.write('sd'+str(max_spd-slw_spd)+'\n')
                ser2.write('sd'+str(-max_spd)+'\n')
            elif rel_pos > 0:
                ser1.write('sd'+str(max_spd)+'\n')
                ser2.write('sd'+str(-max_spd-slw_spd)+'\n')
            else:
                ser1.write('sd'+str(max_spd)+'\n')
                ser2.write('sd-'+str(max_spd)+'\n')

        else:
            #ser1.write('sd0\n')
            #ser2.write('sd0\n')
            if rel_pos > 0:
                ser1.write('sd25\n')
                ser2.write('sd-5\n')

            else:
                ser1.write('sd5\n')
                ser2.write('sd-25\n')
        cv2.imshow('blurred', img)
        #ser1.write('s\n')
        #ser2.write('s\n')
        #ser1_fb = int(ser1.readline()[3:-2])
        #ser2_fb = int(ser2.readline()[3:-2])
        #if not (ser1_fb == '<s:0>\n' and ser2_fb == '<s:0>\n'):
        #    print(ser1_fb)
        #    print(ser2_fb)
    elif int(ko[3]) == 1:
        ret, img = capture.read()
        img = cv2.GaussianBlur(img, (3, 3), 1)
        img_hsv = cv2.cvtColor(img,cv2.COLOR_BGR2HSV)
        ORANGE_MIN = np.array([a, b, c],np.uint8)
        ORANGE_MAX = np.array([d, e, f],np.uint8)
        img_thresholded = cv2.inRange(img_hsv, ORANGE_MIN, ORANGE_MAX)
        cv2.imshow('thresholded', img_thresholded)
        contours, hierarchy = cv2.findContours(img_thresholded, cv2.RETR_TREE,cv2.CHAIN_APPROX_TC89_KCOS)
        bigsize = 0
        biggest = [] #np.ones((2,2),'int')
        for i in range(len(contours)):
            size = cv2.contourArea(contours[i])
            if size > bigsize and size > 35:
                bigsize = size
                biggest = contours[i]
     
        if len(biggest) != 0:
    ##        print(bigsize)
            count = 0
            M = cv2.moments(biggest)
            centroid_x = int(M['m10']/M['m00']) #for direction
            centroid_y = int(M['m01']/M['m00']) #for distance
            #print(centroid_x, centroid_y)
            cv2.circle(img, (centroid_x, centroid_y), 5, (255, 0, 0), -1)
            rel_pos = (centroid_x - 160)/160.0
            max_spd = 30
            slw_spd = int(rel_pos*25)
            mtr1_spd = 0
            mtr2_spd = 0

            if rel_pos < 0:

                ser1.write('sd'+str(max_spd-slw_spd)+'\n')
                ser2.write('sd'+str(-max_spd)+'\n')
            elif rel_pos > 0:
                ser1.write('sd'+str(max_spd)+'\n')
                ser2.write('sd'+str(-max_spd-slw_spd)+'\n')
            else:
                ser1.write('sd'+str(max_spd)+'\n')
                ser2.write('sd-'+str(max_spd)+'\n')

        else:
            #ser1.write('sd0\n')
            #ser2.write('sd0\n')
            if rel_pos > 0:
                ser1.write('sd25\n')
                ser2.write('sd-5\n')

            else:
                ser1.write('sd5\n')
                ser2.write('sd-25\n')
            print(count)
      #      if 100 < count:
       #         drive_fw = 0
        #        while drive_fw < 10:
         #           ser1.write('sd20\n')
          #          ser2.write('sd-20\n')
           #         drive_fw += 1
            
        #ser1.write('s\n')
        #ser2.write('s\n')        
        cv2.imshow('blurred', img)
    else:
        print('EEpa!')
    if cv2.waitKey(10) == 27:
        break
    

ser1.write('fs1\n')
ser2.write('fs1\n')
ser1.write('sd0\n')
ser2.write('sd0\n')

cv2.destroyAllWindows()
cv2.VideoCapture(0).release()
##cv.DestroyWindow("camera2")
