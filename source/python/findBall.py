#!/usr/bin/python

import SimpleCV

img = SimpleCV.Image("pic.png")

img2 = img.colorDistance((243, 55, 9)).invert()
blob = img.findBlobs(60)

blob.show(width=5)

img2.save("pic2.png")
