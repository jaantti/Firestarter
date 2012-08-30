#!usr/bin/python

import serial

#Read from serial
ser = serial.Serial('/dev/ttyACM0', 9600)
ser.readline()

#Write to serial

#Use time.sleep(2) to prevent data loss
ser.write('8')
