#!/usr/bin/python

import serial

ser1 = serial.Serial('/dev/ttyACM0', 115200)
print(ser1.write('?\n'))

######KÕIK ASJAD TAGASTAVAD "2", MITTE id:x #######

