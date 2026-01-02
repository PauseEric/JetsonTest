import Jetson.GPIO as GPIO
import time
import sys
from dynamixelMotor import DXL_Coms #Dynamixel Dependency
import neopixel_spi #RGB LED Strip Dependency
import board


#Initating Load Cell library (HX711)
EMULATE_HX711=False

referenceUnit = 1

if not EMULATE_HX711:
    import gpiod
    from hx711 import HX711
else:
    from emulated_hx711 import HX711

chip = None

def cleanAndExit():
    print("Cleaning...")

    if not EMULATE_HX711:
        chip.close()
        
    print("Bye!")
    sys.exit()

if not EMULATE_HX711:
    chip = gpiod.Chip("0", gpiod.Chip.OPEN_BY_NUMBER)

hx = HX711(dout = 11, pd_sck = 7, chip = chip)

hx.set_reading_format("MSB", "MSB")

#Calibration step for load Cell 

# HOW TO CALCULATE THE REFFERENCE UNIT
# To set the reference unit to 1. Put 1kg on your sensor or anything you have and know exactly how much it weights.
# In this case, 92 is 1 gram because, with 1 as a reference unit I got numbers near 0 without any weight
# and I got numbers around 184000 when I added 2kg. So, according to the rule of thirds:
# If 2000 grams is 184000 then 1000 grams is 184000 / 2000 = 92.
#hx.set_reference_unit(113)

hx.set_reference_unit(referenceUnit)
hx.reset()
hx.tare()
print("Tare done! Load cell setup complete")
#Load Cell setup complete


#Dynamixel Setup
global deviceSerial, B_Rate
deviceSerial = "/dev/ttyUSB0" # Adjust as necessary depending on USB port num 
B_Rate = 1000000 # Baud Rate set for XL-430

dynamixel = DXL_Coms(deviceSerial,B_Rate)
tester = dynamixel.createMotor("tester", motor_number = 1) #Using XL-430 with ID 1
tester.switchMode('position') #Switching to position control mode
tester.enableMotor

motor_list = [tester] #List of motors for easy access 

def MotorPosControl(motorName, movement): #Function to move motor to desired position
    motorName.enableMotor()
    motorName.switchMode('position')
    motorName.writePosition(movement)
    dynamixel.sentAllCmd()
    dynamixel.updateMotorData()

def checkAllPos(): #Function to print all motor positions
    dynamixel.updateMotorData()
    for motor in motor_list:
        print(motor.name)
        print(motor.PRESENT_POSITION_value)
        
#Dynamixel Setup Complete

#RGB LED Strip Setup
#Number of Pixels on each Respective Strip (a, b, c ...)
#a, b refer to onboard LEDS
A_PIXELS= 64 
B_PIXELS= 64
PIXEL_ORDER = neopixel_spi.GRB  # WS2812 usually GRB
LEDDELAY = 0.2

spi = board.SPI()
aPixels = neopixel_spi.NeoPixel_SPI(spi, A_PIXELS, pixel_order=PIXEL_ORDER, auto_write=False)
bPixels = neopixel_spi.NeoPixel_SPI(spi, B_PIXELS, pixel_order=PIXEL_ORDER, auto_write=False)


#Two preset colors are currently Color #03E2FF and Color #FF7C7C
def colorChange(pixel, NUM_PIXELS, status): #status refers to which mode LED is set to (0 --> off, 1 --> Color #03E2FF, 2--> #FF7C7C)
    if (status == 0):
        for i in range(NUM_PIXELS):
            pixel[i] = [0xFFFFFFF]
            pixel.show()
            time.sleep(LEDDELAY)
            print("Color Off")
    elif(status == 1):
        for i in range(NUM_PIXELS):
            pixel[i] = [0x03E2FF]
            pixel.show()
            time.sleep(LEDDELAY)
    elif(status == 2):
        for i in range(NUM_PIXELS):
            pixel[i] = [0xFF7C7C]
            pixel.show()
            time.sleep(LEDDELAY)
    else:
        print("Invalid Status Error")

def main():
    print("Main initated, program running...")
    while (True):
        cmd = input ("Type 1 to open lid, 2 to close lid")
        if (cmd == 1):
            MotorPosControl(tester,683)
        elif(cmd == 2):
            MotorPosControl(tester,0)
        else:
            print("No valid command, please retry")

        if(KeyboardInterrupt):
            print("Code Exiting...")
            exitProtocol()

def exitProtocol(): #resets everything to default 
    print("Executing Exit Protocol")

if __name__ == "__main__":
    main()