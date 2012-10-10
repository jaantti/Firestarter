import serial
import time

#Read from serial
ser1 = serial.Serial('/dev/ttyACM0', 115200)
ser2 = serial.Serial('/dev/ttyACM1', 115200)
speed = '30'
dur = 3
#ser1.write('fs0\n')
#ser2.write('fs0\n')
ser1.write('sd' + speed + '\n')
ser2.write('sd' + speed + '\n')
time.sleep(dur)
#ser1.write('fs1\n')
#ser2.write('fs1\n')
ser1.write('sd0\n')
ser2.write('sd0\n')
  
#ser1.write('st\n')
#ser2.write('st\n')
#circle('50', 3000)
#ser.readline()

#Write to serial

#Use time.sleep(2) to prevent data loss

#ser.write('8')

def circle(speed, dur):
    ser1.write('fs0\n')
    ser2.write('fs0\n')
    ser1.write('sd' + speed + '\n')
    ser2.write('sd' + speed + '\n')
    time.sleep(dur)
    ser1.write('fs1\n')
    ser2.write('fs1\n')
    ser1.write('sd0\n')
    ser2.write('sd0\n')
  
