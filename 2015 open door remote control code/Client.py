import socket
import time
import cPickle
import math

portPrefix = "/dev/ttyACM"
coilPORT = "k"
motorPORTS = ["k","k","k","k"]

def driveFour(spd, angle, rotSpd):
    speed0 = spd * math.sin(math.radians(angle) - math.pi / 4.0) + rotSpd
    speed1 = spd * -math.sin(math.radians(angle) + math.pi / 4.0) + rotSpd
    speed2 = spd * -math.sin(math.radians(angle) - math.pi / 4.0) + rotSpd
    speed3 = spd * math.sin(math.radians(angle) + math.pi / 4.0) + rotSpd

    motorSpeeds = [speed0, speed1, speed2, speed3]

    return motorSpeeds


# create a socket object
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 

# get local machine name
host = socket.gethostname()                           

port = 9999

# connection to hostname on the port.
s.connect((host, port))                               

# Receive no more than 1024 bytes
while True:
    try:
        tm = s.recv(100)

        #s.close()

        print(cPickle.loads(tm))
        time.sleep(0.01)
    except Exception:
        print "Lost connection"
        while True:
            try:
                time.sleep(1)
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                host = socket.gethostname()
                port = 9999
                s.connect((host, port))
                break
            except Exception:
                print "Attempting to reconnect..."