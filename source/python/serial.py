import serial

#Read from serial
ser = serial.Serial('/dev/ttyXXX', BAUD_RATE)
ser.readline()

#Write to serial

#Use time.sleep(2) to prevent data loss
ser.write('8')
