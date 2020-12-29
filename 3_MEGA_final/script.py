import serial
import time
import os

frequency = 1000 # Hertz
duration = 50 # ms
s = serial.Serial('/dev/cu.wchusbserialfd120', 115200)

while True:
    # Serial read section
    msg = s.readline()
    print(msg)
    if 'WRONG' in msg.decode("utf-8"):
        os.system('play -n synth %s sin %s' % (duration/1000, frequency))
