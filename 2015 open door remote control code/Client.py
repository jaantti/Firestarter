import socket
import time
import cPickle
import math
import serial
import sys

portPrefix = "/dev/ttyACM"
coilPORT = "k"
motorPORTS = ["k","k","k","k"]
number = 0

def driveFour(spd, angle, rotSpd):
    speed0 = spd * math.sin(math.radians(angle) - math.pi / 4.0) + rotSpd
    speed1 = spd * -math.sin(math.radians(angle) + math.pi / 4.0) + rotSpd
    speed2 = spd * -math.sin(math.radians(angle) - math.pi / 4.0) + rotSpd
    speed3 = spd * math.sin(math.radians(angle) + math.pi / 4.0) + rotSpd

    motorSpeeds = [speed0, speed1, speed2, speed3]

    return motorSpeeds

for i in range(0,20):
    try:
        print "trying port:"+ str(i)
        ser = serial.Serial(port = portPrefix+str(i), baudrate = 115200, timeout=1)
	##print "port: " + ser.
        print "closing port if open"
        ser.close()
        time.sleep(0.1)
        print "opening port"
        ser.open()
        time.sleep(0.1)
        if ser.isOpen():
            print "writing to port"
            ser.write('?\n')
            print "waiting"
            ##time.sleep(0.7)
            print "reading from port"
            d = ser.readline()
            print "read data:"+d
            curr_id = d[4]
            print "id:"+curr_id
            if (int(curr_id) == 0 or d[0] == 'd'):
                coilPORT = portPrefix + str(i)
                number += 1
            if (int(curr_id) < 5) and (int(curr_id) > 0):
                motorPORTS[int(curr_id)-1] = portPrefix + str(i)
                number += 1
                                
        print "closing port"
        ser.close()
    except Exception:
        print "not found"

print "coilgun: " + coilPORT
print "motors: " + str(motorPORTS)
print "number of devices found: " + str(number) + "\n"

if (number == 5):
    button_data = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
    hat_data = (0,0)
    axes_data = [0.0,0.0,0.0,0.0,0.0]
    move_dir = 0
    move_speed = 0
    rotate_speed = 0

    coil = serial.Serial(port = coilPORT, baudrate = 115200, timeout=1)
    M1 = serial.Serial(port = motorPORTS[0], baudrate = 115200, timeout=1)
    M2 = serial.Serial(port = motorPORTS[1], baudrate = 115200, timeout=1)
    M3 = serial.Serial(port = motorPORTS[2], baudrate = 115200, timeout=1)
    M4 = serial.Serial(port = motorPORTS[3], baudrate = 115200, timeout=1)
        ##if not coil.isOpen(): coil.open()
        ##if not M1.isOpen(): M1.open()
		##if not M2.isOpen(): M2.open()
		##if not M3.isOpen(): M3.open()
		##if not M4.isOpen(): M4.open()


time.sleep(0.1)


# create a socket object
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 

# get local machine name
host = "172.17.201.154"                           

port = 9999

# connection to hostname on the port.
s.connect((host, port))                               

# Receive no more than 1024 bytes
while True:
    tm = s.recv(1000)

    #s.close()

    #print(cPickle.loads(tm))
    data = cPickle.loads(tm)

    speeds = driveFour(data[2], data[0], data[1])
    #print speeds
    coil.write('p\n')
    M1.write('sd'+str(speeds[0])+'\n')
    M2.write('sd'+str(speeds[1])+'\n')
    M3.write('sd'+str(speeds[2])+'\n')
    M4.write('sd'+str(speeds[3])+'\n')
    print data
    if data[3] == 1:
        coil.write('tg\n')
        print 'dribbler go'
    if data[3] == 0:
        coil.write('ts\n')
        print 'dribbler stop'

    if data[4] > 0:
        coil.write('k'+str(data[4])+'\n')

    time.sleep(0.01)

