import cv2
import numpy as np
import cv
switch = 0

capture = cv2.VideoCapture(1)
capture.set(3, 640)
capture.set(4, 320)
def null(*arg):
    pass

cv.NamedWindow('thresh')
cv.NamedWindow('gray')
cv.NamedWindow('aken')
cv.CreateTrackbar('canny', 'gray', 150, 255, null)
cv.CreateTrackbar('threshmax', 'gray', 150, 255, null)
cv.CreateTrackbar('minlinelength', 'gray', 50, 255, null)
cv.CreateTrackbar('maxlinegap', 'gray', 150, 255, null)
cv.CreateTrackbar('hmin', 'aken', 0, 255, null)
cv.CreateTrackbar('smin', 'aken', 0, 255, null)
cv.CreateTrackbar('vmin', 'aken', 0, 255, null)
cv.CreateTrackbar('hmax', 'aken', 50, 255, null)
cv.CreateTrackbar('smax', 'aken', 50, 255, null)
cv.CreateTrackbar('vmax', 'aken', 50, 255, null)
                  
while True:
    a = cv2.getTrackbarPos('canny', 'gray')
    b = cv2.getTrackbarPos('threshmax', 'gray')
    a1 = cv2.getTrackbarPos('minlinelength', 'gray')
    b1 = cv2.getTrackbarPos('maxlinegap', 'gray')
    hmin = cv2.getTrackbarPos('hmin', 'aken')
    smin = cv2.getTrackbarPos('smin', 'aken')
    vmin = cv2.getTrackbarPos('vmin', 'aken')
    hmax = cv2.getTrackbarPos('hmax', 'aken')
    smax = cv2.getTrackbarPos('smax', 'aken')
    vmax = cv2.getTrackbarPos('vmax', 'aken')
    ret, img = capture.read()
    img = cv2.flip(img, -1)
##    img = cv2.GaussianBlur(img, (3, 3),0)    
    thresh = cv2.inRange(img, np.array([hmin, smin, vmin],np.uint8), np.array([hmax, smax, vmax],np.uint8))
##    thresh = thresh[0:640, 0:480]
##    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
##    ret2, thresh = cv2.threshold(img, a, b, cv2.THRESH_BINARY)
##    ret2, thresh2 = cv2.threshold(img, a1, b1, cv2.THRESH_BINARY)
##    thresh = cv2.bitwise_or(thresh, thresh2)
##    thresh = cv2.bitwise_not(thresh)
    edge = cv2.Canny(thresh, a, a*3)
##    roi = img[160:320, 160:240]
    lines = cv2.HoughLinesP(edge, 1, np.pi/180, b, minLineLength = a1, maxLineGap = b1)
    lines = np.array(lines, ndmin = 2)
##    print(str(len(lines)) + "\n")
##    print(lines)
    lines=lines[0]
    for l in lines:
        if(l != None):
            x = 640
            y = 480
            if l[0] != l[2]:
                slope = float((-1*(l[3]-l[1])))/float((l[2]-l[0]))
                if slope > 0:
##                    if  (0*(l[2]-l[0])+l[1]*(l[2]-l[0]))/((l[3]-l[1])-l[0]*(l[3]-l[1])) 
                    if 0 > x*(l[3]-l[1])-l[0]*(l[3]-l[1]) - y*(l[2]-l[0])+l[1]*(l[2]-l[0]):
                        switch = 0
                    elif x*(l[3]-l[1])-l[0]*(l[3]-l[1]) - y*(l[2]-l[0])+l[1]*(l[2]-l[0]) >= 0:
                        switch = 1
                elif 0 > slope:
                    if x*(l[3]-l[1])-l[0]*(l[3]-l[1]) - y*(l[2]-l[0])+l[1]*(l[2]-l[0]) > 0:
                        switch = 0
                    elif 0 >= x*(l[3]-l[1])-l[0]*(l[3]-l[1]) - y*(l[2]-l[0])+l[1]*(l[2]-l[0]):
                        switch = 1
##                elif slope == 0:
##                    y = (pos_x*(l[3]-l[1])-l[0]*(l[3]-l[1]))/((l[2]-l[0])+l[1]*(l[2]-l[0]))
##                    x = (pos_y*(l[2]-l[0])+l[1]*(l[2]-l[0]))/((l[3]-l[1])-l[0]*(l[3]-l[1]))
##                    poly_list = [(0,0), (640, 0), (0, y), (640, y)]
##                print slope
##                print l
##                z = x*(l[3]-l[1])-l[0]*(l[3]-l[1]) - y*(l[2]-l[0])+l[1]*(l[2]-l[0])
##                print(z)
                cv2.line(img, (l[0], l[1]), (l[2],l[3]), (0, 255, 255), 1)
            else:
                print("Oh god vertical line!")
            
    cv2.imshow('thresh', thresh)
    cv2.imshow('gray', img)
##    cv2.imshow('roi', roi)
    if cv2.waitKey(10) == 27:
        break
print switch
cv2.VideoCapture(1).release()
cv2.destroyAllWindows()
    
