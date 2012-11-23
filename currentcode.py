#!/usr/bin/python

import cv2
import numpy as np
import serial
import time


capture = cv2.VideoCapture(1)
#capture.set(3, 320)
#capture.set(4, 240)

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
count_goal = 0
c = 0
switch = 0
was_close = False
fb_list = [(255, 255), (255, 255), (255, 255), (255, 255)]

#colour tuples, hsv min -> hsv max
orange_tty = (5,129,149,28,255,255)
yellow_tty = (29,160,223,32,255,255)
blue_tty = (93,117,89,136,207,172)

orange_t4 = (6, 198, 193, 18, 255, 255)#4, 245, 232, 25, 255, 255)
yellow_t4 = (23, 236, 171, 27, 255, 255)#23, 206, 172, 29, 255, 255)
blue_t4 = (112, 53, 79, 120, 209, 97)#98, 138, 80, 128, 255, 255)
green_t4  = (35, 136, 0, 55, 255, 255)
black_t4 = (17, 0, 0, 41, 255, 138)#0, 0, 66, 53, 255, 134)

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

def green_check(contour, img):
    x,y,w,h = cv2.boundingRect(contour)
    ball_box = img[y:y+h, x:x+w]
    ball_box = cv2.cvtColor(ball_box,cv2.COLOR_BGR2HSV)
    cnt1 = 0.0
    cnt2 = 0
    #not_orange_threshold = cv2.bitwise_not(thresholdedImg(ball_box, orange_t4))
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
        line_green = thresholdedImg(line, green_t4)
        #cv2.imshow('Line_green', line_green)
        line = line[::-1]
        for i in range(len(line)):
            if line_green[i] == 1:
                last_green = 0
            elif (not (line[i][0][1] < 40 and line[i][0][2] > 200)) and last_green < 2: #isn't white and just had green
                return 1
            else:
                last_green += 1
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
        cnt3 = 1
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
                cnt3 = 1
        #print cnt1, cnt2, cnt3, robot_to_ball[i]
    return 0

def goal_find(centroids, ser1, ser2, count_goal):
    global c
    global rel_pos
    global count_goal_find
    if centroids != 0:
        rel_pos = (centroids[0] - 160)/160.0 #horisontal position of blob in vision: -1 left edge, 1 right edge, 0 center
        if rel_pos > 0.15: #blob right of center
            ser1.write('sd-5\n')
            ser2.write('sd-10\n')
            c = 5
        elif rel_pos < -0.15: #blob left of center
            ser1.write('sd10\n')
            ser2.write('sd5\n')
            c = 5
        else:
            ser1.write('sd0\n')
            ser2.write('sd0\n')
            c = 5
            count_goal_find = count_goal_find + 1
        if count_goal_find >= 5 and rel_pos < 0.15 and rel_pos > -0.15:
            count_goal_find = 0
            print 'BOOOOOOOOMMMMM!'
            boom(ser3)
            #raw_input('Press any key to shoot.')
    else: #no blob in view
        if rel_pos > 0: #if blob was last seen on the right, turn right
            ser1.write('sd0\n')
            ser2.write('sd-20\n')
        else: #if blob was last seen on the left, turn left
            ser1.write('sd20\n')
            ser2.write('sd0\n')
        count_goal += 1
    return count_goal

def boom(ser3):
    ser3.write('k1200\n')
    time.sleep(0.04)
            
def drive(centroids, max_spd, slower_by, count, rel_pos):
    '''Drives towards the provided point (assuming the camera faces forward). Turning rate varies depending on x coordinate.'''

    if count == 0 or count > 10:
        print('Saw one!')
        ser1.write('sd0\n')
        ser2.write('sd0\n')
        time.sleep(0.3)
    count = 1
    
    if rel_pos > 0: #blob right of center
        ser1.write('sd'+str(max_spd - int(rel_pos*slower_by))+'\n') #slower wheel speed
        ser2.write('sd'+str(-max_spd)+'\n')
    elif rel_pos < 0: #blob left of center
        ser1.write('sd'+str(max_spd)+'\n')
        ser2.write('sd'+str(-max_spd - int(rel_pos*slower_by))+'\n') #slower wheel speed
    else: #blob exactly in the middle
        ser1.write('sd'+str(max_spd)+'\n')
        ser2.write('sd-'+str(max_spd)+'\n')
        
    return count

#def collisionTimeout(ser1, ser2):
##    Work in progress


def goalTimeout(img_hsv, current_gate, max_spd, slower_by, count_goal):
    global switch
    global rel_pos
    global yellow_tty 
    global blue_tty
    global yellow_t4
    global blue_t4

    if current_gate ==  yellow_t4:
        thresh = blue_t4
    elif current_gate == blue_t4:
        thresh = yellow_t4
    elif current_gate == yellow_tty:
        thresh = blue_tty
    else:
        thresh = yellow_tty
        
    img_thresholded = thresholdedImg(img_hsv, thresh)
    #cv2.imshow('Timeout_gate', img_thresholded)
    centroids = findBlobCenter(img_thresholded, 500, 0)
    
    if centroids != 0:
        if count_goal == 100 or count_goal > 102:
            print('Saw one goal!')
            ser1.write('sd0\n')
            ser2.write('sd0\n')
            time.sleep(0.3)
        count_goal = 100
    
        rel_pos = (centroids[0] - 160)/160.0 #horisontal position of blob in vision: -1 left edge, 1 right edge, 0 center
        if rel_pos > -0.5 and rel_pos < 0.5 and switch != 1:
            switch = 1
        elif switch == 1:
            if centroids[2] < 2000:
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
        return (count_goal + 1)
    
    else:
        if rel_pos > 0: #if blob was last seen on the right, turn right
            ser1.write('sd-10\n')
            ser2.write('sd-15\n')
        else: #if blob was last seen on the left, turn left
            ser1.write('sd15\n')
            ser2.write('sd10\n')
    return (count_goal + 1)
    

def timeout(img_hsv, max_spd, slower_by, count):
    global switch
    global rel_pos
    img_thresholded_yellow = thresholdedImg(img_hsv, yellow_t4)
    img_thresholded_blue = thresholdedImg(img_hsv, blue_t4)
    img_thresholded = img_thresholded_blue + img_thresholded_yellow
    #cv2.imshow('Timeouted', img_thresholded)
    centroids = findBlobCenter(img_thresholded, 500, 0)
    
    if centroids != 0:
        
        if count == 100 or count > 102:
            print('Saw one goal!')
            ser1.write('sd0\n')
            ser2.write('sd0\n')
            time.sleep(0.3)
        count = 100
        
        rel_pos = (centroids[0] - 160)/160.0 #horisontal position of blob in vision: -1 left edge, 1 right edge, 0 center
        if centroids[2] < 1700 and rel_pos > -0.5 and rel_pos < 0.5 and switch != 1:
            switch = 1
        elif switch == 1:
            print 'Gate size' + str(centroids[2])
            if centroids[2] < 3000:
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
            ser1.write('sd-10\n')
            ser2.write('sd-15\n')
            return (count + 1)
            
    else:
        ser1.write('sd-10\n')
        ser2.write('sd-15\n')
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

def ballCheck(ser):
    '''checks if ball is in dribbler
    returns 1 if yes and 0 if no'''
    global c
    ko = int(ser.readline()[3])
    
    if c % 10 == 0:
        ko = 2
        ser1.write('s\n')
        ser2.write('s\n')
        ser1_fb = int(ser1.readline()[3:-2])
        ser2_fb = int(ser2.readline()[3:-2])
        #if not (ser1_fb == '<s:0>\n' and ser2_fb == '<s:0>\n'):
        #    print(ser1_fb)
        #    print(ser2_fb)
        ser.write('ko\n')
        return (ko, ser1_fb, ser2_fb)
    
    ser.write('ko\n')
    return (ko, 255, 255)

#First IR ball check query
ser1.write('ko\n')

#main loop
while True:
    
    c += 1
    ser3.write('p\n')
    ret, img = capture.read() #get the picture to work with
    img_hsv = cv2.cvtColor(img,cv2.COLOR_BGR2HSV) #convert the img to HSV colourspace
    #img = cv2.flip(img, -1) #flip if necessary
    
    ko = ballCheck(ser1) #ball check
    
    if ko[1] != 255:
        flag = 1
        print ko[1], ko[2]
        fb_list.pop()
        fb_list.insert(0, (ko[1], ko[2]))
        for fb in fb_list:
            if not (-4 < fb[0] < 4 and -4 < fb[1] < 4):
                flag = 0
                break
        
        if flag == 1:  
            print 'Stalling'
            fb_list = [(255, 255), (255, 255), (255, 255), (255, 255)]
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
            time.sleep(0.7)
            c = 8
        
    elif ko[0] == 0: #ball not in dribbler
        count_goal = 0
        #print 'Count: ' + str(count)
        if count < 100:
            img_hsv = img_hsv[15:240, 0:320]
            img_hsv = lineDetection(img_hsv, black_t4, 150, 90, 90, 25)
            img_thresholded = thresholdedImg(img_hsv, orange_t4)
            #cv2.imshow('test', img_thresholded)
            centroids = findBlobCenter(img_thresholded, 5, img)
            
            if centroids != 0:
                rel_pos = (centroids[0] - 160)/160.0 #horisontal position of blob in vision: -1 left edge, 1 right edge, 0 center
                if centroids[1] < 70:
                    count = drive(centroids, 60, 15, count, rel_pos)
                    was_close = False
                elif centroids[1] > 200 and centroids[2] > 1500 and not was_close:
                    c = 1
                    ser1.write('sd0\n')
                    ser2.write('sd0\n')
                    time.sleep(0.3)
                    if rel_pos > 0: #blob right of center
                        ser1.write('sd'+str(25 - int(rel_pos*10))+'\n') #slower wheel speed
                        ser2.write('sd'+str(-25)+'\n')
                    elif rel_pos < 0: #blob left of center
                        ser1.write('sd'+str(25)+'\n')
                        ser2.write('sd'+str(-25 - int(rel_pos*10))+'\n') #slower wheel speed
                    else: #blob exactly in the middle
                        ser1.write('sd'+str(25)+'\n')
                        ser2.write('sd-'+str(25)+'\n')
                    print 'Ball close'
                    time.sleep(1)
                    was_close = True
                else:
                    count = drive(centroids, 20, 20, count, rel_pos)
                '''elif rel_pos > 0.15: #if blob was last seen on the right, turn right 
                    if 0.3 > rel_pos > 0.15 :
                        ser1.write('sd-8\n')
                        ser2.write('sd0\n')
                    else:
                        ser1.write('sd-15\n')
                        ser2.write('sd0\n')
                    print 'turning right'
                    count += 1
                elif rel_pos < -0.15: #if blob was last seen on the left, turn left
                    if -0.3 < rel_pos < -0.15:
                        ser1.write('sd0\n')
                        ser2.write('sd8\n')
                    else:
                        ser1.write('sd0\n')
                        ser2.write('sd15\n')
                    print 'turning left'
                    count += 1
                elif count > 10 and rel_pos < 0.15 and rel_pos > -0.15:
                    ser1.write('sd20\n')
                    ser2.write('sd-20\n')
                    print 'GOGOGO'
                    count = 0
                    time.sleep(2)
                else:
                    ser1.write('sd0\n')
                    ser2.write('sd0\n')
                    print 'aiming'
                    count += 1
                    c = 4
'''

            else: #no blob in view
                if rel_pos > 0: #if blob was last seen on the right, turn right
                    ser1.write('sd0\n')
                    ser2.write('sd-20\n')
                else: #if blob was last seen on the left, turn left
                    ser1.write('sd20\n')
                    ser2.write('s10\n')
                count += 1
        
        else:
            img_hsv = img_hsv[0:30, 0:320]
            count = timeout(img_hsv, 40, 20, count)
        
    else: #Ball in dribbler
        count = 0
        #print 'Count_goal: ' + str(count_goal)
        img_hsv = img_hsv[0:30, 0:320]
        current_color = yellow_t4 # <<<<< SIHTVARAVA VARV >>>>>>
        if count_goal < 100:
            img_thresholded = thresholdedImg(img_hsv, current_color)
            #cv2.imshow('goalfinding threshold', img_thresholded)
            centroids = findBlobCenter(img_thresholded, 500, img)
            count_goal = goal_find(centroids, ser1, ser2, count_goal)
        else:
            count_goal = goalTimeout(img_hsv, current_color, 50, 15, count_goal) 
        #drive(centroids, 40, 15, ser1, ser2)
            
            
#   cv2.imshow('Threshed', img_thresholded)
    #if centroids != 0:
    #    cv2.circle(img, (centroids[0], centroids[1]+15), 5, (255, 0, 0), -1) #draws a small blue circle at the biggest blob's center for debugging
    #cv2.imshow('Original image', img) #show img for calibration

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
