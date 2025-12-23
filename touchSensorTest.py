import Jetson.GPIO as GPIO
import time

# Pin Definition (use BOARD numbering mode for physical pin numbers)
output_pin = 7
GPIO.setmode(GPIO.BOARD)
GPIO.setup(7,GPIO.IN)
# Setup pin as output
GPIO.setup(output_pin, GPIO.OUT, initial=GPIO.LOW)

try:
    print("Starting Detection Sequence. Press Ctrl+C to exit")
    while True:
        print("Sensing ")
        print(GPIO.input(7)) # sensing pin 7 as input value
        time.sleep(1) # Wait 1 second
except KeyboardInterrupt:
    print("Exiting Detection Sequence")
finally:
    GPIO.cleanup() # Clean up all GPIO channels