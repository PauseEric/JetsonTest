#include "HX711.h"    //Load Cell Library 
#include <SPI.h>      //SPI communication for MFRC522 (required to use MFRC522)
#include <MFRC522.h>  //RFID Library (use MFRC522 by GithubCommunity v1.4.12, download through library manager)

HX711 loadcellA;  //Right
HX711 loadcellB;  //Left

String cmd;  //received input gets stored here

//Recorded Values
long massA = 0.0;
long massB = 0.0;
int lockState = 0;

//RGB LED Pin for Left Strip
int redPinA = 4;
int greenPinA = 2;
int bluePinA = 3;

//RGB LED Pin for Right Strip
int redPinB = 10;
int greenPinB = 11;
int bluePinB = 9;

//Triggering relayLock can only be for a split second
int relayLock = 53;  //relay (HIGH = Lock Release)
int lockReadPin = 48;  //relay status reader (1 is locked, 0 is lockReleased)

//RFID Module Setup
//SPI MOSI --> Pin 51
//SPI MISO --> Pin 50
//SPI SCK --> Pin 52
int RST_PIN = 12;                //Configurable (originally pin 8, conflict with LEDs)
int SDA_PIN = 13;                //Configurable (originally pin 9, conflict with LEDs) (SPI PIN)
MFRC522 rfid(SDA_PIN, RST_PIN);  //MFRC522 reader
byte nuidPICC[4];                // Init array that will store new NUID (currently unused)

//Loadcell Setup (Divider is the primary calibration, offset is only used in case of consistent inaccuracies with loadcell reading)
const int LOADCELLA_DOUT_PIN = 6;
const int LOADCELLA_SCK_PIN = 5;
const long LOAD_A_OFFSET = 0;
const long LOAD_A_DIVIDER = -420.35;

const int LOADCELLB_DOUT_PIN = 8;
const int LOADCELLB_SCK_PIN = 7;
const long LOAD_B_OFFSET = 0;
const long LOAD_B_DIVIDER = 431.13;

void unlock() {
  digitalWrite(relayLock, HIGH);
  delay(100);
  digitalWrite(relayLock, LOW);
}

void setLeftLEDColor(int r, int g, int b) {
  analogWrite(redPinA, 255 - r);
  analogWrite(greenPinA, 255 - g);
  analogWrite(bluePinA, 255 - b);
}

void setRightLEDColor(int r, int g, int b) {
  analogWrite(redPinB, 255 - r);
  analogWrite(greenPinB, 255 - g);
  analogWrite(bluePinB, 255 - b);
}

void checkSerial() {
  if (Serial.available() > 0) {
    cmd = Serial.readStringUntil('\n');
    if (cmd == "unlock") {
      unlock();
    } else if (cmd == "tare") {//zero loadcells
      loadcellA.tare();
      loadcellB.tare();
    } else if (cmd == "left_led_red") {
      //#FF7C7C Red
      setLeftLEDColor(100, 49, 49);
    } else if (cmd == "right_led_red") {
      //#FF7C7C Red
      setRightLEDColor(100, 49, 49);
    } else if (cmd == "left_led_blue") {
      //#03E2FF Blue
      setLeftLEDColor(3, 226, 255);
    } else if (cmd == "right_led_blue") {
      //#03E2FF Blue
      setRightLEDColor(3, 226, 255);
    } else if (cmd == "left_led_off") {
      setLeftLEDColor(0, 0, 0);
    } else if (cmd == "right_led_off") {
      setRightLEDColor(0, 0, 0);
    } else if (cmd == "led_off") {  //clear
      setLeftLEDColor(0, 0, 0);
      setRightLEDColor(0, 0, 0);
    }
  }
}

void readRFID() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!rfid.PICC_IsNewCardPresent())
    return;
  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial())
    return;
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.print("-1");  //returning -1 means invalid RFID type (not MIFARE Classic type)
    return;
  }
  // Store NUID into nuidPICC array (if need to do comparison and saving logs)
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }
  // printHex(rfid.uid.uidByte, rfid.uid.size);
  printDec(rfid.uid.uidByte, rfid.uid.size);  //print RFID value in dec
  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

//Helper routine to dump a byte array as hex values to Serial.
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

//Helper routine to dump a byte array as dec values to Serial.
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    //Serial.print(' '); Seperate each byte
    Serial.print(buffer[i], DEC);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPI.begin();
  while (!Serial) {
    //waiting for Jetson to Connect
  }
  pinMode(redPinA, OUTPUT);
  pinMode(greenPinA, OUTPUT);
  pinMode(bluePinA, OUTPUT);

  pinMode(redPinB, OUTPUT);
  pinMode(greenPinB, OUTPUT);
  pinMode(bluePinB, OUTPUT);

  pinMode(relayLock, OUTPUT);
  pinMode(lockReadPin, INPUT);

  digitalWrite(redPinA, HIGH);  //high initializes LED A to OFF
  digitalWrite(greenPinA, HIGH);
  digitalWrite(bluePinA, HIGH);

  digitalWrite(redPinB, HIGH);  //high initializes LED B to OFF
  digitalWrite(greenPinB, HIGH);
  digitalWrite(bluePinB, HIGH);

  digitalWrite(relayLock, LOW);

  loadcellA.begin(LOADCELLA_DOUT_PIN, LOADCELLA_SCK_PIN);
  loadcellA.set_scale(LOAD_A_DIVIDER); //recalibrate value every time the loadcell is adjusted

  loadcellB.begin(LOADCELLB_DOUT_PIN, LOADCELLB_SCK_PIN);
  loadcellB.set_scale(LOAD_B_DIVIDER); //recalibrate value every time the loadcell is adjusted

  loadcellA.tare();
  loadcellB.tare();

  rfid.PCD_Init();  // Initialize MFRC522
}

void loop() {
  //Giving Output
  massA = loadcellA.get_units(2);  // get_units(i) reads the loadcell value for 'i' amount of times, the greater the 'i', the slower the program runs
  massB = loadcellB.get_units(2);
  lockState = digitalRead(lockReadPin);  // 0 :: open, 1 :: close
  //Returning Data to Jetson
  char buffer[64];  //add more buffer if needed
  sprintf(buffer, "Lock:%i,L_LC:%ld,R_LC:%ld,RFID:", lockState, massB, massA);
  Serial.print(buffer);
  readRFID(); //process RFID Value, returns "" if no tag is detected
  Serial.print("\n");
  //Check Serial
  checkSerial();
  delay(200);
}

// for calibration of the Loadcell
void loadCellCalibration() {
  loadcellA.set_scale();
  loadcellA.tare();
  Serial.println(loadcellB.get_units(10));

  //LoadCell A Calibrate
  Serial.println("Calibrating Loadcell A \n Place a known weight on Scale A \n Press Any Key to Continue");

  while (Serial.available() == 0) {
    delay(1000);
    Serial.println("Waiting");
  }

  Serial.read();
  double obsVal = loadcellA.get_units(10);
  Serial.println("Observed LoadCellA Value to");
  Serial.println(obsVal);

  delay(1000);
}
