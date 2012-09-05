#!/usr/bin/python

import SimpleCV

'''img = SimpleCV.Image("pic.png")

img2 = img.colorDistance((254, 55, 19))
img3 = img2.binarize(65)
blob = img3.findBlobs()
blob2 = blob.filter([b.isCircle(0.2) for b in blob])

print blob2.area()
print blob.coordinates()
blob2.show(width=5)

img3.save("pic2.png")'''

cam = SimpleCV.Camera(0)
disp = SimpleCV.Display()

while(disp.isNotDone()):
    img = cam.getImage().colorDistance((254, 55, 19)).binarize(65)
    circ = img.findBlobs()
    #img.show(width=5)
    blob = circ.filter([b.isCircle(0.2) for b in blob])
    #blob.show(width=5)
    img.drawCircle((blob[-1].x, blob[-1].y), blob[-1].radius(),SimpleCV.Color.BLUE,3)
    img.show()
