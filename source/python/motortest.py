#!/usr/bin/python

import serial, time

ser1 = serial.Serial('/dev/ttyACM0', 115200)
ser2 = serial.Serial('/dev/ttyACM1', 115200)
ser1.write('fs0\n')
ser2.write('fs0\n')
ser1.write('sd-68\n')
ser2.write('sd70\n')
time.sleep(3)
ser1.write('fs1\n')
ser2.write('fs1\n')
ser1.write('sd0\n')
ser2.write('sd0\n')
