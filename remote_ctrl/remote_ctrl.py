import pygame
import serial
import time

ser1 = serial.Serial('/dev/ttyACM0', 115200) #Right
ser1.write('?\n')
time.sleep(0.5)
if ser1.readline()[4] == 0:
    ser2 = serial.Serial('/dev/ttyACM1', 115200) #Left, negative speeds go forward
else:
    ser1 = serial.Serial('/dev/ttyACM1', 115200)
    ser2 = serial.Serial('/dev/ttyACM0', 115200)

def write_spd(write1, write2):
    ser1.write('sd'+str(write1)+'\n')
    ser2.write('sd'+str(-write2)+'\n')

speed = 60

up = 0
down = 0
left = 0
right = 0
state = {'up':0, 'down':0, 'left':0, 'right':0}

scr = pygame.display.set_mode((1,1))
while(True):
    elist = pygame.event.get()
    for event in elist:
        if event.type == 2 and event.dict.get('key') == 27:
            write_spd(0, 0)
            quit()
        if event.type == 2:
            if event.dict.get('key') == 273:
                state['up'] = 1
            elif event.dict.get('key') == 274:
                state['down'] = 1
            elif event.dict.get('key') == 275:
                state['right'] = 1
            elif event.dict.get('key') == 276:
                state['left'] = 1
        if event.type == 3:
            if event.dict.get('key') == 273:
                state['up'] = 0
            elif event.dict.get('key') == 274:
                state['down'] = 0
            elif event.dict.get('key') == 275:
                state['right'] = 0
            elif event.dict.get('key') == 276:
                state['left'] = 0
    if state['up'] == 1:
        if state['right'] == 1:
            write_spd(0, speed)
        elif state['left'] == 1:
            write_spd(speed, 0)
        else:
            write_spd(speed, speed)
    elif state['left'] == 1:
        write_spd(speed, -speed)
    elif state['right'] == 1:
        write_spd(-speed, speed)
    elif state['down'] == 1:
        write_spd(-speed, -speed)
    else:
        write_spd(0, 0)
    



