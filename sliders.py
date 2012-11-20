

import cv2
import numpy as np

capture = cv2.VideoCapture(1)
cv2.cv.NamedWindow("track", 0)

def null(*arg):
    pass

cv2.cv.CreateTrackbar("hmin", "track", 100, 180, null)  
cv2.cv.CreateTrackbar("smin", "track", 188, 255, null)
cv2.cv.CreateTrackbar("vmin", "track", 222, 255, null)
cv2.cv.CreateTrackbar("hmax", "track", 175, 180, null)
cv2.cv.CreateTrackbar("smax", "track", 255, 255, null)
cv2.cv.CreateTrackbar("vmax", "track", 255, 255, null)

def thresholdedImg(img, a, b, c, d, e, f):
    '''reads an image from the feed and thresholds it using the provided min/max HSV values'''
    img_hsv = cv2.cvtColor(img,cv2.COLOR_BGR2HSV) #convert the img to HSV colourspace
    
    #thresholds for across zero thresholding
    #ORANGE_NOTZERO = np.array([180, e, f],np.uint8)
    #ORANGE_ZERO = np.array([0, b, c],np.uint8)

    thresh_MIN = np.array([a, b, c],np.uint8) #lower threshold
    thresh_MAX = np.array([d, e, f],np.uint8) #higher threshold
    img_thresholded = cv2.inRange(img_hsv, thresh_MIN, thresh_MAX)

    #img_thresholded_2 = cv2.inRange(img_hsv, ORANGE_ZERO, ORANGE_MAX)
    #img_thresholded = img_thresholded_1 + img_thresholded_2 #adds two ranges
    cv2.imshow('thresholded', img_thresholded) #show picture for calibrating
    return img_thresholded

while True:

    a = cv2.getTrackbarPos('hmin', 'track')
    b = cv2.getTrackbarPos('smin', 'track')
    c = cv2.getTrackbarPos('vmin', 'track')
    d = cv2.getTrackbarPos('hmax', 'track')
    e = cv2.getTrackbarPos('smax', 'track')
    f = cv2.getTrackbarPos('vmax', 'track')

    ret, img = capture.read()
    img_thresholded = thresholdedImg(img, a, b, c, d, e, f)
    
    cv2.imshow('color picture', img)
    if cv2.waitKey(10) == 27: #quit on esc
        break
        
cv2.destroyAllWindows()
cv2.VideoCapture(0).release()


