import pygame
import time
import math

deadZone = 0.3
maxSpeed = 220
maxTurnSpeed = 120

##portPrefix = "COM"
portPrefix = "/dev/ttyACM"
coilPORT = "k"
motorPORTS = ["k","k","k","k"]
controllerID = 0

number = 0

def get_angle(a, b):
    if b <= 0:
        if b!=0:
            return math.degrees(math.atan(-a/-b))
        else:
            if a>0:
                return -math.degrees(math.atan(9999999999))
            else:
                return math.degrees(math.atan(9999999999))
    else:
        if a<0:
            if b!=0:
                return 180+math.degrees(math.atan(-a/-b))
            else:
                return 180+math.degrees(math.atan(9999999999))
        else:
            if b!=0:
                return -180+math.degrees(math.atan(-a/-b))
            else:
                return -180+math.degrees(math.atan(9999999999))

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
            if (int(curr_id) == 0):
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

time.sleep(1)

print "initing pygame"
pygame.init()
print "assigning controller"
cont = pygame.joystick.Joystick(controllerID)
print "initing controller"
cont.init()
if cont.get_init():
    print "controller found: " + cont.get_name()
    axes = cont.get_numaxes()
    print "number of axes: " + str(axes)
    buttons = cont.get_numbuttons()
    print "number of buttons: " + str(buttons)
    ##hats = cont.get_numhats()
    ##print "number of hats: " + str(hats)

   
    
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
        
        print "starting cycle"
        while True:
            pygame.event.pump()
            for i in range(buttons):
                button_data[i] = cont.get_button(i)
                ##print "button " + str(i) + ": \t" + str(button_data[i])
            ##for i in range(hats):
                ##hat_data = cont.get_hat(i)
                ##print "hat " + str(i) + ": \t\t" + str(hat_data)
            for i in range(axes):
                axes_data[i] = cont.get_axis(i)
		##print "preaxis " + str(i) + ": \t" + str(axes_data[i])
		##axes_data[i] = float(axes_data[i])/32768.0
                if abs(axes_data[i]) < deadZone:
                    axes_data[i] = 0
                ##print "postaxis " + str(i) + ": \t" + str(axes_data[i])
            if button_data[8] == 1:
                break
            if button_data[6] == 1:
                coil.write('k2000\n')
	    if button_data[7] == 1:
                coil.write('k3000\n')
            if button_data[4] == 1:
                coil.write('ts\n')
            if button_data[5] == 1:
                coil.write('tg\n')
            move_dir = get_angle(axes_data[0], axes_data[1])
            rotate_speed = -maxTurnSpeed*axes_data[2]
            move_speed = math.pow(math.sqrt(math.pow(axes_data[0],2)+math.pow(axes_data[1],2))*0.86,2)*maxSpeed
            print "moving angle: " + str(move_dir)
            print "rotate speed: " + str(rotate_speed)
            print "move speed: " + str(move_speed)

            speeds = driveFour(move_speed, move_dir, rotate_speed)
            coil.write('p\n')
            M1.write('sd'+str(speeds[0])+'\n')
            M2.write('sd'+str(speeds[1])+'\n')
            M3.write('sd'+str(speeds[2])+'\n')
            M4.write('sd'+str(speeds[3])+'\n')
            
            time.sleep(0.1)

        coil.write('ts\n')
	M1.write('sd0\n')
	M2.write('sd0\n')
        M3.write('sd0\n')
        M4.write('sd0\n')    
        coil.close()
        M1.close()
        M2.close()
        M3.close()
        M4.close()
            
    
print "closing controller"
cont.quit()
print "controller closed"
