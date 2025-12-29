import RPi.GPIO as GPIO 
import time
GPIO.setmode(GPIO.BOARD)
touch_pin = 7
GPIO.setup(touch_pin, GPIO.IN)


try: 
    print("Testing Touch Sensor")
    while (True):
        if GPIO.input(touch_pin):
            print("Detected")
        else:
            print("Not Detected")
        time.sleep(0.5)
except KeyboardInterrupt: 
    GPIO.cleanup()