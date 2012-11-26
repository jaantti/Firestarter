#!/usr/bin/python

import cv2
import numpy as np
import serial
import time


capture = cv2.VideoCapture(1)
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
rel_pos_ball = 0
count = 0
count_goal_find = 0
count_goal = 0
c = 0
switch = 0
was_close = False
#fb_list = [(255, 255), (255, 255), (255, 255), (255, 255)]
fb = (0, 0)
spd1 = 0
spd2 = 0

#colour tuples, hsv min -> hsv max
orange_tty = (8, 138, 43, 17, 255, 255)
yellow_tty = (34, 196, 92, 41, 255, 165)
blue_tty = (92, 66, 32, 108, 205, 57)
green_tty = (61, 138, 53, 73, 255, 210)
black_tty = (63, 83, 0, 79, 143, 95)

orange_t4 = (6, 198, 193, 18, 255, 255)
yellow_t4 = (23, 236, 171, 27, 255, 255)
blue_t4 = (112, 53, 79, 120, 209, 97)
green_t4  = (35, 136, 0, 55, 255, 255)
black_t4 = (17, 0, 0, 41, 255, 138)

ser3.write('c\n')

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
    for i in range(len(contours)-1, -1, -1):
        size = cv2.contourArea(contours[i])
        if size >= minSize and size > bigsize:
            #green_check(contours_1[i], img)
            #if field_edge(contours[i], img) == 1:
            #    print 'Off the field'
            #    continue
            #elif minSize < 300 and edge_check(contours[i], img) == 1:
            #    print 'Behind a line!'
            #    continue
            bigsize = size
            biggest = i
            
    if biggest != -1: #if there is a blob
        #cv2.imshow('Greens', green_check(contours[biggest], img[15:240, 0:320]))
        
        x,y,w,h = cv2.boundingRect(contours[biggest])
        centroid_x = x+w/2 #for direction
        centroid_y = y+h/2 #for distance
        #print bigsize, centroid_y
        #print(centroid_x, centroid_y) #for debugging
        return (centroid_x, centroid_y, bigsize)
    else:
        return 0

def goal_find(centroids, ser1, ser2, count_goal):
    global c
    global rel_pos
    global count_goal_find
    if centroids != 0:
        rel_pos = (centroids[0] - 160)/160.0 #horisontal position of blob in vision: -1 left edge, 1 right edge, 0 center
        if rel_pos > 0.15: #blob right of center
            write_spd(0, -20)
            c = 5
        elif rel_pos < -0.15: #blob left of center
            write_spd(20, 0)
            c = 5
        else:
            write_spd(0, 0)
            c = 5
            count_goal_find = count_goal_find + 1
        if count_goal_find >= 7 and rel_pos < 0.15 and rel_pos > -0.15:
            count_goal_find = 0
            print 'BOOOOOOOOMMMMM!'
            boom(ser3)
            #raw_input('Press any key to shoot.')
    else: #no blob in view
        if rel_pos > 0: #if blob was last seen on the right, turn right
            write_spd(0, -30)
        else: #if blob was last seen on the left, turn left
            write_spd(30, 0)
        count_goal += 1
    return count_goal

def boom(ser3):
    ser3.write('k1000\n')
    time.sleep(0.1)
            
def write_spd(write1, write2):
    global spd1
    global spd2
    ser1.write('sd'+str(write1)+'\n')
    ser2.write('sd'+str(write2)+'\n')
    spd1 = write1
    spd2 = write2

def drive(centroids, max_spd, slower_by, count, rel_pos):
    '''Drives towards the provided point (assuming the camera faces forward). Turning rate varies depending on x coordinate.'''

    if count == 0 or count > 10:
        print('Saw one!')
        write_spd(0, 0)
        time.sleep(0.1)
    count = 1
    
    if rel_pos > 0: #blob right of center
        write_spd(max_spd - rel_pos*slower_by, -max_spd)
    elif rel_pos < 0: #blob left of center
        write_spd(max_spd, -max_spd - int(rel_pos*slower_by))
    else: #blob exactly in the middle
        write_spd(max_spd, -max_spd)
        
    return count


def goalTimeout(img_hsv, current_gate, max_spd, slower_by, count_goal):
    global switch
    global rel_pos
    global yellow_tty 
    global blue_tty
    global yellow_tty
    global blue_tty

    if current_gate ==  yellow_tty:
        thresh = blue_tty
    else:
        thresh = yellow_tty
        
    img_thresholded = thresholdedImg(img_hsv, thresh)
    #cv2.imshow('Timeout_gate', img_thresholded)
    centroids = findBlobCenter(img_thresholded, 500, 0)
    
    if centroids != 0:
        if count_goal == 100 or count_goal > 102:
            print('Saw one goal!')
            write_spd(0, 0)
            time.sleep(0.1)
        count_goal = 100
    
        rel_pos = (centroids[0] - 160)/160.0 #horisontal position of blob in vision: -1 left edge, 1 right edge, 0 center
        if rel_pos > -0.5 and rel_pos < 0.5 and switch != 1:
            switch = 1
        elif switch == 1:
            if centroids[2] < 2000:
                if rel_pos > 0: #blob right of center
                    write_spd(max_spd - int(rel_pos*slower_by), -max_spd)
                elif rel_pos < 0: #blob left of center
                    write_spd(max_spd, -max_spd - int(rel_pos*slower_by))
                else: #blob exactly in the middle
                    write_spd(max_spd, -max_spd)
            else:
                switch = 0
                'New position aquired'
                return 0
        else:
            if rel_pos > 0: #if blob was last seen on the right, turn right
                write_spd(-15, -20)
            else: #if blob was last seen on the left, turn left
                write_spd(20, 15)
        return (count_goal + 1)
    
    else:
        if rel_pos > 0: #if blob was last seen on the right, turn right
            write_spd(-15, -20)
        else: #if blob was last seen on the left, turn left
            write_spd(20, 15)
    return (count_goal + 1)
    

def timeout(img_hsv, max_spd, slower_by, count):
    global switch
    global rel_pos
    img_thresholded_yellow = thresholdedImg(img_hsv, yellow_tty)
    img_thresholded_blue = thresholdedImg(img_hsv, blue_tty)
    img_thresholded = img_thresholded_blue + img_thresholded_yellow
    #cv2.imshow('Timeouted', img_thresholded)
    centroids = findBlobCenter(img_thresholded, 500, 0)
    
    if centroids != 0:
        
        if count == 100 or count > 102:
            print('Saw one goal!')
            write_spd(0, 0)
            time.sleep(0.1)
        count = 100
        
        rel_pos = (centroids[0] - 160)/160.0 #horisontal position of blob in vision: -1 left edge, 1 right edge, 0 center
        if centroids[2] < 1700 and rel_pos > -0.5 and rel_pos < 0.5 and switch != 1:
            switch = 1
        elif switch == 1:
            print 'Gate size' + str(centroids[2])
            if centroids[2] < 3000:
                if rel_pos > 0: #blob right of center
                    write_spd(max_spd - int(rel_pos*slower_by), -max_spd)
                elif rel_pos < 0: #blob left of center
                    write_spd(max_spd, -max_spd - int(rel_pos*slower_by))
                else: #blob exactly in the middle
                    write_spd(max_spd, -max_spd)
            else:
                switch = 0
                'New position aquired'
                return 0
        else:
            write_spd(-30, -30)
            return (count + 1)
            
    else:
        write_spd(-30, -30)
    return (count + 1)
        
def lineDetection(img, colour, a, b, minLineLength, maxLineGap):
    thresh = thresholdedImg(img, colour)
    #cv2.imshow('lines', thresh)
    edge = cv2.Canny(thresh, a, a*3)
    lines = cv2.HoughLinesP(edge, 1, np.pi/180, b, minLineLength = minLineLength, maxLineGap = maxLineGap)
    lines = np.array(lines, ndmin = 2)
    lines=lines[0]
    
    for l in lines:
        if(l != None):
            if l[2]!=l[0] and l[3]!=l[1]:
                # y = a * x + b
                a = (l[3]-l[1])/float((l[2]-l[0]))
                b = l[1] - (l[0]*(l[3]-l[1]))/float((l[2]-l[0]))
                #list for fillPoly
                points = []
                #print a, b
                if 0 <= b <= 240:
                    A = [0, b]
                    points.append([0, 0])
                elif b > 240:
                    A = [(240-b)/a, 240]
                    points.append([0, 240])
                    points.append([0, 0])
                else:
                    A = [(0-b)/a, 0]                    
                chk1 = (a*320+b)
                #print chk1
                if 0 <= chk1 <= 240:
                    B = [320, chk1]
                    points.append([320, 0])
                elif chk1 < 0:
                    B = [(0-b)/a, 0]                    
                else:
                    B = [(240-b)/a, 240]
                    points.append([320, 0])
                    points.append([320, 240])                    
                    
                points.append(B)
                points.append(A)
                #print points
                array = np.array(points, 'int32')
                cv2.fillConvexPoly(img, array, (255, 0, 255))
    return img

def ballCheck():
    '''checks if ball is in dribbler
    returns 1 if yes and 0 if no'''
    global c
    ko = int(ser1.readline()[3])
    
    if c % 10 == 0:
        #ko = 2
        ser1.write('s\n')
        ser2.write('s\n')
        ser1_fb = int(ser1.readline()[3:-2])
        ser2_fb = int(ser2.readline()[3:-2])
        #if not (ser1_fb == '<s:0>\n' and ser2_fb == '<s:0>\n'):
        #    print(ser1_fb)
        #    print(ser2_fb)
        ser1.write('gb\n')
        return (ko, ser1_fb, ser2_fb)
    
    ser1.write('gb\n')
    return (ko, 255, 255)

def list_der(in_list):
    out_list = []
    for i in range(len(in_list)-1):
        out_list.append(in_list[i+1]-in_list[i])
    return out_list

#First IR ball check query
ser1.write('gb\n')

#main loop
while True:
    c += 1
    ser3.write('p\n')
    ret, img = capture.read() #get the picture to work with
    img_hsv = cv2.cvtColor(img,cv2.COLOR_BGR2HSV) #convert the img to HSV colourspace
    #img = cv2.flip(img, -1) #flip if necessary
    
    ko = ballCheck() #ball check

    if ko[1] != 255:
	green = thresholdedImg(img_hsv, green_tty)
	#print np.sum(green)/255
	if np.sum(green) < 30720:
	    print 'Stalling'
            #fb_list = [(255, 255), (255, 255), (255, 255), (255, 255)]
            ser1.write('sd0\n')
            ser2.write('sd0\n')
            time.sleep(0.2)
            ser1.write('sd-50\n')
            ser2.write('sd50\n')
            time.sleep(1)
            ser1.write('sd0\n')
            ser2.write('sd0\n')
            time.sleep(0.2)
            ser1.write('sd50\n')
            ser2.write('sd50\n')
            time.sleep(0.6)
            ser3.write('k\n')
            c += 1

    if ko[0] == 0: #ball not in dribbler
        count_goal = 0
        #print 'Count: ' + str(count)
        if count < 100:
            img_hsv = img_hsv[15:240, 0:320]
            img_hsv = lineDetection(img_hsv, black_tty, 150, 90, 90, 25)
            img_thresholded = thresholdedImg(img_hsv, orange_tty)
            #cv2.imshow('test', img_thresholded)
            centroids = findBlobCenter(img_thresholded, 5, img)
            
            if centroids != 0:
               
                rel_pos = (centroids[0] - 160)/160.0 #horisontal position of blob in vision: -1 left edge, 1 right edge, 0 center

                if centroids[1] < 70:
                    count = drive(centroids, 90, 40, count, rel_pos)
                    was_close = False
                elif centroids[1] > 200 and centroids[2] > 1500 and not was_close:
                    c = 1
                    write_spd(0, 0)
                    time.sleep(0.1)
                    if rel_pos > 0: #blob right of center
                        write_spd(25 - int(rel_pos*20), -25)
                    elif rel_pos < 0: #blob left of center
                        write_spd(25, -25 - int(rel_pos*20))
                    else: #blob exactly in the middle
                        write_spd(25, -25)
                    print 'Ball close'
                    time.sleep(0.6)
                    was_close = True
                else:
                    count = drive(centroids, 25, 25, count, rel_pos)
                
            else: #no blob in view

                if rel_pos > 0: #if blob was last seen on the right, turn right
                    write_spd(0, -30)
                else: #if blob was last seen on the left, turn left
                    write_spd(30, 0)
                count += 1
        
        else:
            img_hsv = img_hsv[0:30, 0:320]
            count = timeout(img_hsv, 90, 50, count)
        
    else: #Ball in dribbler
        count = 0
        #print 'Count_goal: ' + str(count_goal)
        img_hsv = img_hsv[0:30, 0:320]
        current_color = yellow_tty # <<<<< SIHTVARAVA VARV >>>>>>
        if count_goal < 100:
            img_thresholded = thresholdedImg(img_hsv, current_color)
            centroids = findBlobCenter(img_thresholded, 500, img)
            count_goal = goal_find(centroids, ser1, ser2, count_goal)
        else:
            count_goal = goalTimeout(img_hsv, current_color, 50, 20, count_goal) 
            
    #if centroids != 0:
    #    cv2.circle(img, (centroids[0], centroids[1]+15), 5, (255, 0, 0), -1) #draws a small blue circle at the biggest blob's center for debugging
    #cv2.imshow('Original image', img) #show img for calibration

    #print spd2, spd1, 'wanted speed'

    if cv2.waitKey(10) == 27: #quit on esc
        break

#on quit, stop the motors
ser1.write('fs1\n')
ser2.write('fs1\n')
ser1.write('sd0\n')
ser2.write('sd0\n')

ser3.write('d\n')

#clean and release
cv2.destroyAllWindows()
cv2.VideoCapture(0).release()
