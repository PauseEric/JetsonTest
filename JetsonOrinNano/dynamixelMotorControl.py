#import Jetson.GPIO as GPIO
import time
import sys
from dynamixelMotor import DXL_Coms #Dynamixel Dependency

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
def main():
    print("Main initated, program running...")
 
    try:
        
        while (True):
            
            print ("Running Main Loop...")
            
           
            cmd = int(input ("Type 1 to open lid, 2 to close lid, 3 to unlock Lock"))
            if (cmd == 1): #Open Lid
                MotorPosControl(tester,683) #turns 60 degrees from origin ((4096/360)*60 --> 683, 683 ticks equates to 60 degree turn)
            elif(cmd == 2): #Close Lid
                MotorPosControl(tester,0)
            else: 
                print("No valid command, please retry")
            
            
    except(KeyboardInterrupt):
        print("Code Exiting... (Keyboard Interrupt Triggered)")
        exitProtocol()


def exitProtocol(): #resets everything to default 
    print("Executing Exit Protocol")
    
    #RED.stop()
    #GREEN.stop()
    #BLUE.stop()
   # GPIO.cleanup()


if __name__ == "__main__":
    main()