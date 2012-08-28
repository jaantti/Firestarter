from SimpleCV import *
cam = Camera()

while (1):
        i = cam.getImage()
        i.show()
