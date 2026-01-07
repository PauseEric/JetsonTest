#include "HX711.h" //Load Cell Library 

HX711 loadcellA; //Right
HX711 loadcellB; //Left

//Recorded Values
long massA = 0.0;
long massB = 0.0;

int lockState = 0;


String cmd; //received input get stored here

//Pins
//Pin 2 for Arduino Mega malfun ctioning, avoid use (only on current board)

//RGB LED Pin for Strip A
int redPinA = 4;
int greenPinA = 12; 
int bluePinA = 3;

//RGB LED Pin for Strip B
int redPinB = 10;
int greenPinB = 11;
int bluePinB = 9;


int relayLock = 50; //relay (HIGH = Lock Release)
//Triggering relayLock can only be for a spilt second
int lockReadPin = 48; //relay status reader (1 is locked, 0 is lockReleased)

const int LOADCELLA_DOUT_PIN = 6;
const int LOADCELLA_SCK_PIN = 5;

const int LOADCELLB_DOUT_PIN = 8;
const int LOADCELLB_SCK_PIN = 7;

const long LOADAOFFSET = 0;
const long LOADADIVIDER = -420.35;

const long LOADBOFFSET = 0;
const long LOADBDIVIDER = 431.13;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) {
    //waiting for Jetson to Connect
  }

  pinMode(redPinA,  OUTPUT);
  pinMode(greenPinA, OUTPUT);
  pinMode(bluePinA, OUTPUT);

  pinMode(redPinB,  OUTPUT);
  pinMode(greenPinB, OUTPUT);
  pinMode(bluePinB, OUTPUT);

  pinMode(relayLock, OUTPUT);
  pinMode(lockReadPin, INPUT);

  digitalWrite(redPinA, HIGH); //high initializes LED A to OFF
  digitalWrite(greenPinA, HIGH);
  digitalWrite(bluePinA, HIGH);

  digitalWrite(redPinB, HIGH); //high initializes LED B to OFF
  digitalWrite(greenPinB, HIGH);
  digitalWrite(bluePinB, HIGH);

  digitalWrite(relayLock, LOW);

  loadcellA.begin(LOADCELLA_DOUT_PIN, LOADCELLA_SCK_PIN);
  loadcellA.set_scale(LOADADIVIDER);
  //loadcellA.set_offset(LOADAOFFSET);

  loadcellB.begin(LOADCELLB_DOUT_PIN, LOADCELLB_SCK_PIN);
  loadcellB.set_scale(LOADBDIVIDER);
  //loadcellB.set_offset(LOADBOFFSET);
  loadcellA.tare();
  loadcellB.tare();
}

void loop() {
  // put your main code here, to run repeatedly:
  //loadCellCalibration();

  //setColor(255, 255, 0);
  //Serial.println("Blue");
  // digitalWrite(relayLock, LOW);
  //Serial.println(digitalRead(lockReadPin));

  // digitalWrite(relayLock,LOW);
  // Serial.println(digitalRead(lockReadPin));
  //   setColor(100, 49, 49);
  //   Serial.println("#FF7C7C");
  //   delay(2000);
  // //  setColor(3,226,255);
  // //  Serial.println("#03E2FF");
  // //  delay(2000);

 

  //Giving Output
  massA = loadcellA.get_units(2);// get_units(i) reads the loadcell value for 'i' amount of times, the greater the 'i', the slower the program runs
  massB = loadcellB.get_units(2);
  lockState = digitalRead(lockReadPin);
  char buffer[64]; //add more buffer if needed
  sprintf(buffer, "Lock:%i,L_LC:%ld,R_LC:%ld", lockState, massB, massA);
  Serial.println(buffer);

  //Receiving Input
  if (Serial.available() > 0) {
    cmd = Serial.readStringUntil('\n');
    if (cmd == "unlock") {
      unlock();
    } else if (cmd == "tare") {
      loadcellA.tare();
      loadcellB.tare();
    } else if (cmd == "led_red") { //#FF7C7C Red
      setColorA(100, 49, 49);
      setColorB(100, 49, 49);
    } else if (cmd == "led_blue") { //#03E2FF Blue
      setColorA(3, 226, 255);
      setColorB(3, 226, 255);
    } else if (cmd == "led_off") { //clear
      setColorA(0, 0, 0);
      setColorB(0, 0, 0);
    }
  } else {
    //Serial.println("Waiting for Jetson");
     
  }

}

void unlock() {
  digitalWrite(relayLock, HIGH);
  delay(100);
  digitalWrite(relayLock, LOW);
}

void setColorA (int r, int g, int b) {
  analogWrite(redPinA, 255 - r);
  analogWrite(greenPinA, 255 - g);
  analogWrite(bluePinA, 255 - b);
}

void setColorB (int r, int g, int b) {
  analogWrite(redPinB, 255 - r);
  analogWrite(greenPinB, 255 - g);
  analogWrite(bluePinB, 255 - b);
}


void loadCellCalibration(){
  /*
     Instructions for Calibration:
     Run either loadcell A or B independently (comment out the unused Loadcell part)
     first set scale to have no input
     Put a known mass on the loadcell when prompted with "waiting" in Serial Monitor
     Record the value that is returnd
     Divide the returned value by the known mass
     Put the divided value into setScale()
     Run the program again using different known mass to check the accuracy
  */
  loadcellA.set_scale();
  loadcellA.tare();
  //  loadcellB.set_scale();
  //  loadcellB.tare();
  //Serial.println(loadcellA.get_units(10));
  Serial.println(loadcellB.get_units(10));
  
  //LoadCell A Calibrate
  Serial.println("Calibrating Loadcell A \n Place a known weight on Scale A \n Press Any Key to Continue");
  while (Serial.available() == 0) {
    delay (1000);
    Serial.println("Waiting");
  }
  Serial.read();
  double obsVal = loadcellA.get_units(10);
  Serial.println("Observed LoadCellA Value to");
  Serial.println(obsVal);


  //  //LoadCell B Calibrate
  //  Serial.println("Calibrating Loadcell B \n Place a known weight on Scale B \n Press Any Key to Continue");
  //  while (Serial.available() == 0) {
  //    delay (1000);
  //    Serial.println("Waiting");
  //  }
  //  Serial.read();
  //  double obsVal = loadcellB.get_units(10);
  //  Serial.println("Observed LoadCellB Value to");
  //  Serial.println(obsVal);

  delay(1000);

}