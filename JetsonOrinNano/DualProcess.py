#import Jetson.GPIO as GPIO
import time
import sys
from dynamixelMotor import DXL_Coms #Dynamixel Dependency
from UIdemo import windowUI #UI Dependency
from PyQt5 import QtWidgets #UI 
from PyQt5.QtWidgets import QApplication, QMainWindow #UI
import serial #For Arduino Serial Communication

import multiprocessing
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

#Initialize UI Window
app = QApplication(sys.argv)
win = windowUI()

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


def main():
    print("Main initated, program running...")
    win.show() #diplay window
    time.sleep(2)#wait for UI to load

    #receivedString = ""
    


    try:
        
        while (True):
            #win.b1.clicked.connect(unlockLock)
            print ("Running Main Loop...")
            
            #Serial Connection to Arduino to get sensor data
            arduino.write("lockCheck".encode()) #Sends command to Arduino to check lock status
            receivedString= arduino.readline().decode('utf-8').rstrip() #Reads Arduino Serial Output
            LockSensor= int(receivedString)
            
            if(LockSensor == 1):
                win.lockStatusLabel.setText("Status: Lock is Engaged")
                win.lockStatusLabel.adjustSize()
            else:
                win.lockStatusLabel.setText("Status: Lock is Disengaged")
                win.lockStatusLabel.adjustSize()

            arduino.write("loadACheck".encode()) #Sends command to Arduino to check load cell A (Right) status
            receivedString= arduino.readline().decode('utf-8').rstrip() #Reads Arduino Serial Output
            RightLoad= float(receivedString)
            win.loadRightlabel.setText("Load Cell A (Right) Value: " + str(RightLoad))

            arduino.write("loadBCheck".encode()) #Sends command to Arduino to check load cell status
            receivedString= arduino.readline().decode('utf-8').rstrip() #Reads Arduino
            LeftLoad= float(receivedString)
            win.loadLeftlabel.setText("Load Cell B (Left) Value: " + str(LeftLoad))
            
            
            cmd = int(input ("Type 1 to open lid, 2 to close lid, 3 to unlock Lock"))
            if (cmd == 1): #Open Lid *LED change to Color 1)
                MotorPosControl(tester,683) #turns 60 degrees from origin ((4096/360)*60 --> 683, 683 ticks equates to 60 degree turn)
                
                print("color Switch Green")
                arduino.write("Green".encode())
            elif(cmd == 2): #Close Lid *LED turn off
                MotorPosControl(tester,0)
            
            elif(cmd == 3): #Change LED Color to Color 2
            
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

def lockStatus():
    print("Checking Lock Status...")
    win.notif.setText("Status: Lock Status Button Pressed")

def exitProtocol(): #resets everything to default 
    print("Executing Exit Protocol")
    
    #RED.stop()
    #GREEN.stop()
    #BLUE.stop()
   # GPIO.cleanup()


if __name__ == "__main__":
    main()