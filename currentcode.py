#!/usr/bin/python

import cv2
import numpy as np
import serial
import time


capture = cv2.VideoCapture(1)
cv2.cv.NamedWindow("track", 0)
capture.set(3, 320)
capture.set(4, 240)

ser1 = serial.Serial('/dev/ttyACM0', 115200) #Right
ser2 = serial.Serial('/dev/ttyACM1', 115200)
ser1.write('?\n')
ser2.write('?\n')
time.sleep(0.5)
check1 = str(ser1.readline())

if len(check1) > 2 and check1[4] == str(0): #ser1 at correct device
    if ser2.readline()[4] == str(1): #ser2 at correct device
        ser3 = serial.Serial('/dev/ttyACM2', 115200) #coilgun
    else:
        ser3 = serial.Serial('/dev/ttyACM1', 115200)
        ser2 = serial.Serial('/dev/ttyACM2', 115200) #Left, negative speeds go forward
elif len(check1) > 2 and check1[4] == str(1): #ser1 is the opposite motor
    if ser2.readline()[4] == str(0): #ser2 is opposite motor
        ser1 = serial.Serial('/dev/ttyACM1', 115200)
        ser2 = serial.Serial('/dev/ttyACM0', 115200)
        ser3 = serial.Serial('/dev/ttyACM2', 115200)
    else:
        ser3 = serial.Serial('/dev/ttyACM1', 115200)
        ser2 = serial.Serial('/dev/ttyACM0', 115200)
        ser1 = serial.Serial('/dev/ttyACM2', 115200)
else:
    if ser2.readline()[4] == str(1): #ser2 at correct device
        ser3 = serial.Serial('/dev/ttyACM0', 115200)
        ser1 = serial.Serial('/dev/ttyACM2', 115200)
    else:
        ser3 = serial.Serial('/dev/ttyACM0', 115200)
        ser1 = serial.Serial('/dev/ttyACM1', 115200)
        ser2 = serial.Serial('/dev/ttyACM2', 115200)
        
#initialization of global variables
rel_pos = 0
count = 0
count_goal_find = 0
c = 0
switch = 0

ser3.write('e\n')
ser3.write('c\n')

#making trackbars
def null(*arg):
    pass


cv2.cv.CreateTrackbar("hmin", "track", 100, 180, null)  
cv2.cv.CreateTrackbar("smin", "track", 188, 255, null)
cv2.cv.CreateTrackbar("vmin", "track", 222, 255, null)
cv2.cv.CreateTrackbar("hmax", "track", 175, 180, null)
cv2.cv.CreateTrackbar("smax", "track", 255, 255, null)
cv2.cv.CreateTrackbar("vmax", "track", 255, 255, null)

def thresholdedImg(img, colour):
    '''reads an image from the feed and thresholds it using the provided min/max HSV values'''

    thresh_MIN = np.array([colour[0], colour[1], colour[2]],np.uint8) #lower threshold
    thresh_MAX = np.array([colour[3], colour[4], colour[5]],np.uint8) #higher threshold
    img_thresholded = cv2.inRange(img, thresh_MIN, thresh_MAX)
    
    #cv2.imshow('thresholded', img_thresholded) #show picture for calibrating
    return img_thresholded

def findBlobCenter(img_thresholded, minSize, img):
    '''using a binary image finds the center of the biggest blob and returns it's coordinates as a tuple'''
    contours, hierarchy = cv2.findContours(img_thresholded, cv2.RETR_TREE,cv2.CHAIN_APPROX_TC89_KCOS) #Find contours, reverse the list
    #finds the biggest blob
    bigsize = 0
    biggest = -1
    for i in range(len(contours)):
        size = cv2.contourArea(contours[i])
        if size >= minSize and size > bigsize:
            #green_check(contours_1[i], img)
            if field_edge(contours[i], img) == 1:
                print 'Off the field'
                continue
            elif minSize < 300 and edge_check(contours[i], img) == 1:
                print 'Behind a line!'
                continue
            bigsize = size
            biggest = i
    if biggest != -1: #if there is a blob
        #cv2.imshow('Greens', green_check(contours[biggest], img[15:240, 0:320]))
        #print bigsize
        x,y,w,h = cv2.boundingRect(contours[biggest])
        centroid_x = x+w/2 #for direction
        centroid_y = y+h/2 #for distance
        #print(centroid_x, centroid_y) #for debugging
        return (centroid_x, centroid_y, bigsize)
    else:
        return 0

def green_check(contour, img):
    x,y,w,h = cv2.boundingRect(contour)
    ball_box = img[y:y+h, x:x+w]
    ball_box = cv2.cvtColor(ball_box,cv2.COLOR_BGR2HSV)
    cnt1 = 0.0
    cnt2 = 0
    not_orange_threshold = cv2.bitwise_not(thresholdedImg(ball_box, (5,179,149,28,255,255)))
    #green_threshold = thresholdedImg(ball_box, 42, 67, 126, 66, 183, 255)
    #thresholded_img = not_orange_threshold - green_threshold
    #ball_box = cv2.cvtColor(ball_box,cv2.COLOR_HSV2BGR)
    return not_orange_threshold
    #print cnt1/cnt2
    #print "___________________________"
    #if cnt1/cnt2 <= 0.25

def field_edge(contour, img):
    x,y,w,h = cv2.boundingRect(contour)    
    lines = [img[y+h:240, x:x+1], img[y+h:240, x+w/2:x+w/2+1], img[y+h:240, x+w-1:x+w]]
    for line in lines:
        last_green = 2
        line = cv2.cvtColor(line,cv2.COLOR_BGR2HSV)
        line_green = thresholdedImg(line, (42, 67, 126, 66, 183, 255))
        #cv2.imshow('Line_green', line_green)
        line = line[::-1]
        for i in range(len(line)):
            if line_green[i] == 1:
                last_green = 0
            elif not (line[i][0][1] < 40 and line[i][0][2] > 200) and last_green < 2: #is white and just had green
                return 1
    return 0
            
def edge_check(contour, img):
    x,y,w,h = cv2.boundingRect(contour)    
    lines = [img[y+h:240, x:x+1], img[y+h:240, x+w/2:x+w/2+1], img[y+h:240, x+w-1:x+w]]
    for line in lines:
        line = cv2.cvtColor(line,cv2.COLOR_BGR2GRAY)
        line = line[::-1]
        #cv2.imshow('Line', robot_to_ball)
        cnt1 = 0
        cnt2 = 0
        cnt3 = 2
        #print robot_to_ball
        for i in range(len(line)-2):
            if cnt2 >= 2:
                return 1
            elif cnt1 >= 4 and line[i] <= 120:
                cnt2 = cnt2 + 1
            elif line[i] >= 225:
                cnt1 = cnt1 + 1
            elif cnt3 >= 0 and cnt1 >= 4:
                cnt3 = cnt3 - 1
            else:
                cnt1 = 0
                cnt3 = 2
        #print cnt1, cnt2, cnt3, robot_to_ball[i]
    return 0

def goal_find(centroids, ser1, ser2):
    global rel_pos
    global count_goal_find
    if centroids != 0:
        rel_pos = (centroids[0] - 160)/160.0 #horisontal position of blob in vision: -1 left edge, 1 right edge, 0 center
        if rel_pos > 0.1: #blob right of center
            ser1.write('sd-5\n')
            ser2.write('sd-10\n')
        elif rel_pos < -0.1: #blob left of center
            ser1.write('sd10\n')
            ser2.write('sd5\n')
        else:
            ser1.write('sd0\n')
            ser2.write('sd0\n')
            count_goal_find = count_goal_find + 1
        if rel_pos < 0.1 and rel_pos > -0.1 and count_goal_find >= 10:
            count_goal_find = 0
            print 'BOOOOOOOOMMMMM!'
            boom(ser3)
            #raw_input('Press any key to shoot.')
    else: #no blob in view
        if rel_pos > 0: #if blob was last seen on the right, turn right
            ser1.write('sd-15\n')
            ser2.write('sd-20\n')
        else: #if blob was last seen on the left, turn left
            ser1.write('sd20\n')
            ser2.write('sd15\n')

def boom(ser3):
    ser3.write('k\n')
    time.sleep(0.05)
    ser3.write('e\n')
    ser3.write('c\n')
            
def drive(centroids, max_spd, slower_by, count):
    '''Drives towards the provided point (assuming the camera faces forward). Turning rate varies depending on x coordinate.'''
    global rel_pos
    if centroids != 0:
        if count == 0 or count > 10:
            print('Saw one!')
            ser1.write('sd0\n')
            ser2.write('sd0\n')
            time.sleep(0.3)
        count = 1
        rel_pos = (centroids[0] - 160)/160.0 #horisontal position of blob in vision: -1 left edge, 1 right edge, 0 center

        if rel_pos > 0: #blob right of center
            ser1.write('sd'+str(max_spd - int(rel_pos*slower_by))+'\n') #slower wheel speed
            ser2.write('sd'+str(-max_spd)+'\n')
        elif rel_pos < 0: #blob left of center
            ser1.write('sd'+str(max_spd)+'\n')
            ser2.write('sd'+str(-max_spd - int(rel_pos*slower_by))+'\n') #slower wheel speed
        else: #blob exactly in the middle
            ser1.write('sd'+str(max_spd)+'\n')
            ser2.write('sd-'+str(max_spd)+'\n')

    else: #no blob in view
        if rel_pos > 0: #if blob was last seen on the right, turn right
            ser1.write('sd-10\n')
            ser2.write('sd-10\n')
        else: #if blob was last seen on the left, turn left
            ser1.write('sd10\n')
            ser2.write('sd10\n')
        count += 1
    return count

def timeout(img_hsv, max_spd, slower_by, count):
    global switch
    global rel_pos
    img_thresholded_yellow = thresholdedImg(img_hsv, (29,160,223,32,255,255))
    img_thresholded_blue = thresholdedImg(img_hsv, (93,117,89,136,207,172))
    img_thresholded = img_thresholded_blue + img_thresholded_yellow
    cv2.imshow('Timeouted', img_thresholded)
    centroids = findBlobCenter(img_thresholded, 500, 0)
    
    if count == 200 or count > 202:
        print('Saw one goal!')
        ser1.write('sd0\n')
        ser2.write('sd0\n')
        time.sleep(0.3)
    count = 200
    
    if centroids != 0:
        rel_pos = (centroids[0] - 160)/160.0 #horisontal position of blob in vision: -1 left edge, 1 right edge, 0 center
        if centroids[2] < 1700 and rel_pos > -0.75 and rel_pos < 0.75 and switch != 1:
            switch = 1
        elif switch == 1:
            if centroids[2] < 4000:
                if rel_pos > 0: #blob right of center
                    ser1.write('sd'+str(max_spd - int(rel_pos*slower_by))+'\n') #slower wheel speed
                    ser2.write('sd'+str(-max_spd)+'\n')
                elif rel_pos < 0: #blob left of center
                    ser1.write('sd'+str(max_spd)+'\n')
                    ser2.write('sd'+str(-max_spd - int(rel_pos*slower_by))+'\n') #slower wheel speed
                else: #blob exactly in the middle
                    ser1.write('sd'+str(max_spd)+'\n')
                    ser2.write('sd-'+str(max_spd)+'\n')
            else:
                switch = 0
                'New position aquired'
                return 0
    else:
        if rel_pos > 0: #if blob was last seen on the right, turn right
            ser1.write('sd-10\n')
            ser2.write('sd-15\n')
        else: #if blob was last seen on the left, turn left
            ser1.write('sd15\n')
            ser2.write('sd10\n')
    return (count + 1)
        
    
def ballCheck(ser):
    '''checks if ball is in dribbler
    returns 1 if yes and 0 if no'''
    ko = ser.readline()
    ser.write('ko\n')
    return int(ko[3])

#Turn off automatic stopping
#ser1.write('fs0\n')
#ser2.write('fs0\n')

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
    img_hsv = cv2.cvtColor(img,cv2.COLOR_BGR2HSV) #convert the img to HSV colourspace
    #img = cv2.flip(img, -1) #flip if necessary
    ko = ballCheck(ser1) #ball check

    if ko == 0: #ball not in dribbler
        img_hsv = img_hsv[15:240, 0:320]
        #print count
        if count < 200:
            img_thresholded = thresholdedImg(img_hsv, (5,179,149,28,255,255))
            cv2.imshow('test', img_thresholded)
            #5,129,149,28,255,255) #orange ball (TTY)
            #5, 188, 160, 15, 255, 255) #orange ball (evening)
            centroids = findBlobCenter(img_thresholded, 0, img)
            if centroids != 0 and centroids[1] < 70:
                count = drive(centroids, 50, 15, count)
            else:
                count = drive(centroids, 30, 10, count)
        else:
            count = timeout(img_hsv, 20, 10, count)
        

    else: #Ball in dribbler
        img_hsv = img_hsv[0:20, 0:320]
        img_thresholded = thresholdedImg(img_hsv, (93,117,89,136,207,172))
        cv2.imshow('test', img_thresholded)
        #29,160,223,32,255,255) #Yellow TTY spordihoone
        #93,117,89,136,207,172) #Blue TTY spordihoone
        #26, 175, 255, 31, 255, 255) #Yellow goal (daytime)  
        #22, 255, 255, 27, 255, 255) #Yellow goal (Evening)
        #121, 140, 73, 130, 190, 108) #blue goal (Evening)
        centroids = findBlobCenter(img_thresholded, 500, img)
        goal_find(centroids, ser1, ser2)
        #drive(centroids, 40, 15, ser1, ser2)
        
    #cv2.imshow('Threshed', img_thresholded)
    if centroids != 0:
        cv2.circle(img, (centroids[0], centroids[1]+15), 5, (255, 0, 0), -1) #draws a small blue circle at the biggest blob's center for debugging
    cv2.imshow('Original image', img) #show img for calibration

    if cv2.waitKey(10) == 27: #quit on esc
        break

#on quit, stop the motors
ser1.write('fs1\n')
ser2.write('fs1\n')
ser1.write('sd0\n')
ser2.write('sd0\n')

ser3.write('k\n')

#clean and release
cv2.destroyAllWindows()
cv2.VideoCapture(0).release()