from SimpleCV import *
cam = Camera(0)

while (1):
        i = cam.getImage()
        i.show()
