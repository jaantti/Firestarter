import serial

ser1 = serial.Serial('/dev/ttyACM0', 115200)

while(True):
	ser1.write('sd30\n')
	ser1.write('gb\n')
	print ser1.readline()
	
