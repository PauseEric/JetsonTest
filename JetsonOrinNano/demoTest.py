#import Jetson.GPIO as GPIO
import time
import sys
from dynamixelMotor import DXL_Coms #Dynamixel Dependency
from UIdemo import windowUI #UI Dependency
from PyQt5 import QtWidgets #UI 
from PyQt5.QtWidgets import QApplication, QMainWindow #UI
import serial #For Arduino Serial Communication

#import neopixel_spi #RGB LED Strip Dependency
#import board #(Creates conflict with Jetson.GPIO --> forced to use TEGRA_SOC mode for Jetson.GPIO)
#GPIO.cleanup() #Reset GPIO settings before running script
#mode = GPIO.getmode
#print(mode)

#Connect via Arduino Serial
arduino=serial.Serial('/dev/ttyACM0', 9600) #Adjust ACM number as necessary

#GPIO.setmode(GPIO.BOARD) #Setting GPIO mode to BOARD
#Variables to Store Arduino States

LeftLoad = 0.0
RightLoad = 0.0;
LockSensor = 0;
#Dynamixel Setup
global deviceSerial, B_Rate
deviceSerial = "/dev/ttyUSB0" # Adjust as necessary depending on USB port num 
B_Rate = 1000000 # Baud Rate set for XL-430 (default 1000000)
M_ID =1 #Motor ID for XL-430

dynamixel = DXL_Coms(deviceSerial,B_Rate)
tester = dynamixel.createMotor("tester", motor_number = M_ID) #Using XL-430 with ID 1
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



'''Using PWM Control on Jetson

#RBG LED not using NeoPixel Setup
#Pin uses default GPIO pins configured to PWM (using Pin 33,35,37)

LED_RPIN= 15
LED_GPIN= 32
LED_BPIN= 33
GPIO.setup ([LED_RPIN, LED_GPIN, LED_BPIN], GPIO.OUT)
RED = GPIO.PWM(LED_RPIN, 1000)  # Set frequency to 1 kHz
GREEN = GPIO.PWM(LED_GPIN, 1000)  # Set frequency to 1 kHz
BLUE = GPIO.PWM(LED_BPIN, 1000)  # Set frequency to 1 kHz

RED.start(0)  # Start PWM with 0% duty cycle (off)
GREEN.start(0)
BLUE.start(0)

def _map(x, in_min, in_max, out_min, out_max):
    return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)

def setColor(r,g,b):
    RED.ChangeDutyCycle(_map(r, 0, 255, 0, 100))
    GREEN.ChangeDutyCycle(_map(g, 0, 255, 0, 100))
    BLUE.ChangeDutyCycle(_map(b, 0, 255, 0, 100))
'''
''' NeoPixel Color Change Function
#Number of Pixels on each Respective Strip (a, b, c ...)
#a, b refer to onboard LEDS
A_PIXELS= 64 
B_PIXELS= 64
PIXEL_ORDER = neopixel_spi.GRB  # WS2812 usually GRB
LEDDELAY = 0.2
spi= board.SPI()


aPixels = neopixel_spi.NeoPixel_SPI(spi, A_PIXELS, pixel_order=PIXEL_ORDER, auto_write=False)
#bPixels = neopixel_spi.NeoPixel_SPI(spi, B_PIXELS, pixel_order=PIXEL_ORDER, auto_write=False)


#Two preset colors are currently Color #03E2FF and Color #FF7C7C
def colorChange(pixel, NUM_PIXELS, status): #status refers to which mode LED is set to (0 --> off, 1 --> Color #03E2FF, 2--> #FF7C7C)
    if (status == 0):
        aPixels.fill(0)
        aPixels.show()
        time.sleep(LEDDELAY)
        print("Color Off")
    elif(status == 1):
        for i in range(NUM_PIXELS):
            aPixels[i] = 0x03E2FF
        aPixels.show()
        time.sleep(LEDDELAY)
    elif(status == 2):
        for i in range(NUM_PIXELS):
            aPixels[i] = 0xFF7C7C
        aPixels.show()
        time.sleep(LEDDELAY)    
    else:
        print("Invalid Status Error")
'''

def main():
    print("Main initated, program running...")

    app = QApplication(sys.argv)
    win = windowUI()
    
    

  

    win.show() #end of window display
    
    try:
        while (True):
           

            
            cmd = int(input ("Type 1 to open lid, 2 to close lid, 3 to unlock Lock"))
            if (cmd == 1): #Open Lid *LED change to Color 1)
                MotorPosControl(tester,683) #turns 60 degrees from origin ((4096/360)*60 --> 683, 683 ticks equates to 60 degree turn)
                #colorChange(aPixels, A_PIXELS, 1)
                # colorChange(bPixels, B_PIXELS, 1)
                print("color Switch Green")
                arduino.write("Green".encode())
            elif(cmd == 2): #Close Lid *LED turn off
                MotorPosControl(tester,0)
            # colorChange(aPixels, A_PIXELS, 0)
            # colorChange(bPixels, B_PIXELS, 0)
            elif(cmd == 3): #Change LED Color to Color 2
            # colorChange(aPixels, A_PIXELS, 2)
            #colorChange(bPixels, B_PIXELS, 2)
                print("unlock lock")
                arduino.write("unlock".encode()) #Sends command to Arduino to change color
                print("Unlock Command Sent")
                
            else: 
                print("No valid command, please retry")
            
    except(KeyboardInterrupt):
        print("Code Exiting... (Keyboard Interrupt Triggered)")
        sys.exit(app.exec_())
        exitProtocol()

def unlockLock():
    print("Unlocking Lock...")
    arduino.write("unlock".encode()) #Sends command to Arduino to change color

def exitProtocol(): #resets everything to default 
    print("Executing Exit Protocol")
    
    #RED.stop()
    #GREEN.stop()
    #BLUE.stop()
   # GPIO.cleanup()


if __name__ == "__main__":
    main()