import RPi.GPIO as GPIO

from mfrc522 import SimpleMFRC522
import time 

reader= SimpleMFRC522()

while (True):
    try:
        print("Hold a tag near the reader")
        id, text = reader.read()
        print("ID: %s\nText: %s" % (id,text))
        time.sleep(1)
    except KeyboardInterrupt:
        GPIO.cleanup()
        raise
        break 


    