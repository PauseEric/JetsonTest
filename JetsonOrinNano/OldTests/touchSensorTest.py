import Jetson.GPIO as GPIO
import time

# Pin Definition (use BOARD numbering mode for physical pin numbers)

GPIO.setmode(GPIO.BOARD)
GPIO.setup(11,GPIO.IN)



try:
    print("Starting Detection Sequence. Press Ctrl+C to exit")
    while True:
        print("Sensing ")
        if (GPIO.input(11) == GPIO.HIGH):
            print("Touch Detected!")
        else:
            print("No Touch Detected.")
     
        time.sleep(1) # Wait 1 second
except KeyboardInterrupt:
    print("Exiting Detection Sequence")
finally:
    GPIO.cleanup() # Clean up all GPIO channels