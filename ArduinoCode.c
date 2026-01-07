#include "HX711.h" //Load Cell Library 

HX711 loadcellA; //Right
HX711 loadcellB; //Left 

//Recorded Values 
double massA = 0.0;
double massB = 0.0;

int lockState=0;


String cmd; 
//Pins

//Pin 2 for Arduino could be malfunctioning, avoid using

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
const long LOADADIVIDER = -431.137931034;

const long LOADBOFFSET = 0;
const long LOADBDIVIDER = 427.66;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
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
  
  digitalWrite(relayLock,LOW);

  loadcellA.begin(LOADCELLA_DOUT_PIN, LOADCELLA_SCK_PIN);
  loadcellA.set_scale(LOADADIVIDER);
  //loadcellA.set_offset(LOADAOFFSET);

  loadcellB.begin(LOADCELLB_DOUT_PIN, LOADCELLB_SCK_PIN);
  loadcellB.set_scale(LOADBDIVIDER);
  //loadcellB.set_offset(LOADBOFFSET);
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
  
  
  massA= loadcellA.get_units(10); 
  massB= loadcellB.get_units(10);
  lockState= digitalRead(lockReadPin);
  
  if (Serial.available() > 0) {
    cmd= Serial.readStringUntil('\n');
    if (cmd == "unlock"){
      unlock();
      Serial.println("unlocked");
    }else if (cmd == "lockCheck"){
      Serial.println(lockState);
    }else if(cmd =="loadACheck"){
      Serial.println(massA);
    }else if (cmd == "loadBCheck"){
      Serial.println(massB);  
    }else if (cmd == "Green"){
      setColor(0,255,0);
      //Serial.println("Color Green");
    }else{
      //Serial.println("No Valid Command Found");
    } 
  }else{
    //Serial.println("Waiting for Jetson");
  }
  
}


void unlock(){
  digitalWrite(relayLock, HIGH);
  delay(100);
  digitalWrite(relayLock, LOW);
}



void setColor (int r, int g, int b) {

  analogWrite(redPinA, 255-r);
  analogWrite(greenPinA, 255-g);
  analogWrite(bluePinA, 255-b);

  analogWrite(redPinB, 255-r);
  analogWrite(greenPinB, 255-g);
  analogWrite(bluePinB, 255-b);
  Serial.println("Color Set");
}




void loadCellCalibration(){
  /*
   * Instructions for Calibration:
   * Run either loadcell A or B independently (comment out the unused Loadcell part)
   * first set scale to have no input
   * Put a known mass on the loadcell when prompted with "waiting" in Serial Monitor 
   * Record the value that is returnd 
   * Divide the returned value by the known mass
   * Put the divided value into setScale()
   * Run the program again using different known mass to check the accuracy
   */
  //loadcellA.set_scale();
  loadcellB.set_scale();
  //loadcellA.tare();
  loadcellB.tare();
  //Serial.println(loadcellA.get_units(10));
  Serial.println(loadcellB.get_units(10));
  /*
  //LoadCell A Calibrate
  Serial.println("Calibrating Loadcell A \n Place a known weight on Scale A \n Press Any Key to Continue");  
  while (Serial.available()==0){
    delay (1000);
    Serial.println("Waiting");
  }
  Serial.read();
  double obsVal = loadcellA.get_units(10);
  Serial.println("Observed LoadCellA Value to");
  Serial.println(obsVal);
  delay(1000);
  */
  //LoadCell B Calibrate
  Serial.println("Calibrating Loadcell B \n Place a known weight on Scale B \n Press Any Key to Continue");
    while (Serial.available()==0){  
    delay (1000);
    Serial.println("Waiting");
  }
  double obsVal = loadcellB.get_units(10);
  Serial.println("Observed LoadCellB Value to");  
  Serial.println(obsVal);
  
  
}