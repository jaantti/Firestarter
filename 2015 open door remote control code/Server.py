import socket                                         
import time
import pygame
import time
import math
import cPickle


deadZone = 0.3
maxSpeed = 220
maxTurnSpeed = 120

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

print "---- inti controller ---\n"
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

button_data = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
hat_data = (0, 0)
axes_data = [0.0, 0.0, 0.0, 0.0, 0.0]
move_dir = 0
move_speed = 0
rotate_speed = 0

print "---- inti server ---\n"
# create a socket object
print "Starting server."
print "Getting socket."
serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# get local machine name

print"Getting hostname."
host = socket.gethostname()                           

port = 9999                                           
print "Hosting on port: %d." %port
# bind to the port
serversocket.bind((host, port))

# queue up to 5 requests
serversocket.listen(5)

print "\nReady. Waiting for connection..."


# establish a connection
clientsocket, addr = serversocket.accept()
print "Got a connection from %s" % str(addr)
print "--- starting controller cycle ---\n"
tribblerStatus = 0
coilTimeout = 0
quitFlag = -1
tribTimeou = -1
print "Running...\n"
while True:
    try:
        if quitFlag == 1:
            sendList = [0, 0, 0, 0, 0, 0, -1]
            break
        #Stuff to be sent in such a sceme: angle, rotation, speed, tribbler status, kick value
        #print axes_data
        sendList = []
        kickValue = -1
        pygame.event.pump()
        for i in range(buttons):
            button_data[i] = cont.get_button(i)
        for i in range(axes):
            axes_data[i] = cont.get_axis(i)
            if abs(axes_data[i]) < deadZone:
                axes_data[i] = 0
                    ##print "postaxis " + str(i) + ": \t" + str(axes_data[i])
            if button_data[7] == 1:
                quitFlag = 1
                break
            if button_data[1] == 1 and coilTimeout == 0:
                #print 'k2000\n'
                kickValue = 2000
                coilTimeout = 60
            if button_data[0] == 1 and coilTimeout == 0:
                #print 'k3000\n'
                kickValue = 3000
                coilTimeout = 60
            if button_data[4] == 1:
                #print 'ts\n'
                tribblerStatus = 0
            if button_data[5] == 1:
                #print 'tg\n'
                tribblerStatus = 1
                tribTimeou = 300
            if coilTimeout > 0:
                coilTimeout = coilTimeout - 1
            if tribTimeou > 0:
                tribTimeou = tribTimeou - 1
            if tribTimeou == 0:
                tribblerStatus = 0
            move_dir = get_angle(axes_data[0], axes_data[1])
            rotate_speed = -maxTurnSpeed*axes_data[2]
            move_speed = math.pow(math.sqrt(math.pow(axes_data[0], 2)+math.pow(axes_data[1], 2))*0.86, 2)*maxSpeed
            #print "moving angle: " + str(move_dir)
            #print "rotate speed: " + str(rotate_speed)
            #print "move speed: " + str(move_speed)

        sendList.append(move_dir)
        sendList.append(rotate_speed)
        sendList.append(move_speed)
        sendList.append(tribblerStatus)
        sendList.append(kickValue)


        data = cPickle.dumps(sendList)

        #print data

        #print cPickle.loads(data)

        #print buffer
        clientsocket.send(data)

        time.sleep(0.07)
        #currentTime = time.ctime(time.time()) + "\r\n"
    except Exception:
        print "Connection to client %s lost" % str(addr)
        serversocket.settimeout(15)
        while True:
            try:
                clientsocket, addr = serversocket.accept()
                print "Got a connection from %s" % str(addr)
                break
            except Exception:
                print "Timed out..."
                quitFlag = 1
                break

print "--- closing controller ---"
cont.quit()
print "controller closed"

print "--- closing connection ---"
clientsocket.close()
print "connection closed"
