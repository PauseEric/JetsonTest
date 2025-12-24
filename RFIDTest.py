from time import sleep
import sys
#import Jetson.GPIO as GPIO
from Jetson_MFRC522 import SimpleMFRC522
reader = SimpleMFRC522()

'''
This script reads RFID tags using the MFRC522 reader.
MFRC522 should follow the connection below
SDA  - Pin 24 (GPIO8)
SCK  - Pin 23 (GPIO11)
MOSI - Pin 19 (GPIO10)
MISO - Pin 21 (GPIO9)
IRQ  - Pin 13 (May not be neccesary)
GND  - (Ground)
RST  - Pin 22 (GPIO25)
3.3V - (3.3V)


Note:
The command below needs to be run on the NVIDIA Jetson Orin Nano device  
    sudo /opt/nvidia/jetson-io/jetson-io.py
Make sure then to enable the SPI interface and reboot the device.

Download the following library to interact with the MFRC522 RFID reader:
pip install Jetson-MFRC522

'''

try:
    while True:
        print("Hold a tag near the reader")
        id, text = reader.read()
        print("ID: %s\nText: %s" % (id,text))
        sleep(5)
except KeyboardInterrupt:
    GPIO.cleanup()
    raise
