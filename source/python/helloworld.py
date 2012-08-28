from SimpleCV import *
cam = Camera(1)

while (1):
        i = cam.getImage()
        i.show()
