#include <FastLED.h>

/* =========================
   STRIP LED COUNTS
   ========================= */
#define RFRONT 19
#define LFRONT 19
#define RBACK  19
#define LBACK  19

/* =========================
   MATRIX
   ========================= */
#define MATRIX_WIDTH  8
#define MATRIX_HEIGHT 8
#define MATRIX_LEDS   (MATRIX_WIDTH * MATRIX_HEIGHT)

/* =========================
   PINS
   ========================= */
#define PIN_RFRONT   22
#define PIN_LFRONT   24
#define PIN_RBACK    23
#define PIN_LBACK   25
#define PIN_MATRIX   26

CRGB frontR[RFRONT];
CRGB frontL[LFRONT];
CRGB backR[RBACK];
CRGB backL[LBACK];
CRGB matrix[MATRIX_LEDS];

//Serial Input Stored in this variable
String cmd = "";

/* =========================
   SERIAL PARSER
   ========================= */
#define SERIAL_BUF_SIZE 64
char serialBuf[SERIAL_BUF_SIZE];
uint8_t serialPos = 0;
bool serialOverflow = false;


/* =========================
   MATRIX TEXT
   ========================= */
#define MATRIX_TEXT_MAX 32
char matrixText[MATRIX_TEXT_MAX] = "READY";

bool printing = false;
/* =========================
   STATE
   ========================= */
enum Direction {GIVEWAY, MOVINGOFF, MOVING, RESPONSE, TURN_LEFT, TURN_RIGHT, ARRIVE, STANDBY, NEUTRAL };
Direction currentState = GIVEWAY;

/* =========================
   TIMERS
   ========================= */
unsigned long lastBlink  = 0;
unsigned long lastBreath = 0;
unsigned long lastScroll = 0;

/* =========================
   Counters for checking LED Animation
   ========================= */

//General
bool animationComplete = false; //check whether the current animation cycle is finished
long prevR = 0;// previous RGB Value
long prevG = 0;
long prevB = 0;

//breathing
bool initialBreathCall = true;
bool dimming = true; //default breath animation starts from full brightness dims
int breathCycleTime = 600; //intialize breath cycle period (millisecond)
int breathSteps = 20; //number of steps it takes from full brightness to low brightness
int breathCount = 0;

//blinking
bool initialBlinkCall = true;
bool delayComplete = false;
int blinkCycleCount = 1;
int blinkCount = 0; // number of blinked cycles
int blinkCycleTime = 1800; //initalize blink cycle period (millisecond)
int blinkRestTime = 600; //delay time for the blinking animation


//Scroll
const char* message = "TESTING";
int textWidth = strlen(message) * 6; // Width of text + spacing
int scrollOffset = 0;
String displaytxt = "";
String reversetxt = "";
int blinkDelay = 500; //default blink delay set to 500ms


int scrollX = MATRIX_WIDTH;



/* =========================
   MOVING DECLARATIONS
   ========================= */
void handleSerial();
void processCommand(char* cmd);
void setMatrixText(const char* text);
void updateBlink();
void updateBreathing();


// --- Your 5x7 Font Definition (A-Z) ---
const uint8_t font5x7[26][5] = {
  {0x00, 0x7E, 0x11, 0x11, 0x7E}, {0x00, 0x36, 0x49, 0x49, 0x7F}, // A, B
  {0x00, 0x22, 0x41, 0x41, 0x3E}, {0x00, 0x3E, 0x41, 0x41, 0x7F}, // C, D
  {0x00, 0x41, 0x49, 0x49, 0x7F}, {0x00, 0x01, 0x09, 0x09, 0x7F}, // E, F
  {0x00, 0x32, 0x51, 0x41, 0x3E}, {0x00, 0x7F, 0x08, 0x08, 0x7F}, // G, H
  {0x00, 0x41, 0x7F, 0x41, 0x00}, {0x00, 0x3F, 0x41, 0x40, 0x20}, // I, J
  {0x00, 0x63, 0x14, 0x08, 0x7F}, {0x00, 0x40, 0x40, 0x40, 0x7F}, // K, L
  {0x7F, 0x02, 0x04, 0x02, 0x7F}, {0x00, 0x7F, 0x08, 0x04, 0x7F}, // M, N
  {0x00, 0x3E, 0x41, 0x41, 0x3E}, {0x00, 0x06, 0x09, 0x09, 0x7F}, // O, P
  {0x00, 0x7E, 0x61, 0x41, 0x3E}, {0x00, 0x66, 0x19, 0x09, 0x7F}, // Q, R
  {0x00, 0x32, 0x49, 0x49, 0x26}, {0x01, 0x01, 0x7F, 0x01, 0x01}, // S, T
  {0x00, 0x3F, 0x40, 0x40, 0x3F}, {0x1F, 0x20, 0x40, 0x20, 0x1F}, // U, V
  {0x3F, 0x40, 0x38, 0x40, 0x3F}, {0x63, 0x14, 0x08, 0x14, 0x63}, // W, X
  {0x07, 0x08, 0x70, 0x08, 0x07}, {0x43, 0x45, 0x49, 0x51, 0x61}  // Y, Z
};

/* =========================
   MATRIX XY MAPPING (SERPENTINE)
   ========================= */

uint16_t XY(uint8_t x, uint8_t y) {
  if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return MATRIX_TEXT_MAX;
  if (y & 0x01) { // Odd rows are reversed for serpentine panels
    return (y * MATRIX_WIDTH) + ((MATRIX_WIDTH - 1) - x);
  } else { // Even rows are normal
    return (y * MATRIX_WIDTH) + x;
  }
}
/* =========================
   NON-BLOCKING SERIAL
   ========================= */
void handleSerial() {
  if (Serial.available() > 0) {
    cmd = Serial.readStringUntil('\n');
    if (cmd.length() > 0) {
      if (cmd == "move_off") {
        currentState = MOVINGOFF;
      } else if (cmd == "move_response") {
        currentState = RESPONSE;
      } else if (cmd == "turning_left") {
        currentState = TURN_LEFT;
      } else if (cmd == "turning_right") {
        currentState = TURN_RIGHT;
      } else if (cmd == "giveway") {
        currentState = GIVEWAY;
      } else if (cmd == "arrived") {
        currentState = ARRIVE;
      } else if (cmd == "amr_number") {

      } else if (cmd == "standby") {
        currentState = STANDBY;
      } else if (cmd == "moving") {
        currentState = MOVING;
      } else if (cmd.substring(0, 6) == "print:") {
        printing = true;
        reversetxt = cmd.substring(6);
        displaytxt="";
        for (int i = reversetxt.length() - 1; i >= 0; i--) {
          displaytxt += reversetxt[i];
        }
        scrollOffset = -displaytxt.length() * 6;
      }
      //need to add another mode called "standby"
    }
  }
}
//void processCommand(char* cmd) {
//  // D:X
//
//
//
//  if (cmd[0] == 'D' && cmd[1] == ':' && cmd[2]) {
//    switch (cmd[2]) {
//      case 'F': currentState = MOVING;    break;
//      case 'B': currentState = STOP;   break;
//      case 'L': currentState = TURN_LEFT;  break;
//      case 'R': currentState = TURN_RIGHT; break;
//      case 'S': currentState = GIVEWAY;       break;
//    }
//    Serial.println(cmd);
//  }
//  // T:TEXT
//  else if (cmd[0] == 'T' && cmd[1] == ':') {
//    setMatrixText(cmd + 2);
//  }
//}

/* =========================
   SETUP
   ========================= */
void setBreathing( CRGB leds[], int ledcnt, long r, long g, long b, bool lead, int cycleCount) {
  //bool lead is used to track which LED Strip to reference as the parent while every other strip follows the lead
  if (lead == true) {
    if (initialBreathCall == true) {
      initialBreathCall = false;
      prevR = 0;
      prevG = 0;
      prevB = 0;
      breathCount = -1;
    } else {
      EVERY_N_MILLISECONDS(breathCycleTime / breathSteps) {
        if ((abs(prevR) < (r / breathSteps)) && (abs(prevG) < (g / breathSteps)) && (abs(prevB) < (b / breathSteps))) {
          dimming = false;
          breathCount = breathCount + 1;
          Serial.println("switched to bright");
        } else if (prevR >= r && prevG >= g && prevB >= b) {
          dimming = true;
          breathCount = breathCount + 1;
          Serial.println("switched to dim ");
        }
        if (dimming == true) {
          if ((prevR - (r / breathSteps)) <= 0) {
            prevR = 0;
          } else {
            prevR = abs(prevR - (r / breathSteps));
          }
          if ((prevG - (g / breathSteps)) <= 0) {
            prevG = 0;
          } else {
            prevG = abs(prevG - (g / breathSteps));
          }
          if ((prevB - (b / breathSteps)) <= 0) {
            prevB = 0;
          } else {
            prevB = abs(prevB - (b / breathSteps));
          }
        } else {
          prevR = prevR + (r / breathSteps);
          prevB = prevB + (b / breathSteps);
          prevG = prevG + (g / breathSteps);
        }
      }
      if (breathCount == cycleCount) {
        initialBreathCall = true;
        animationComplete = true;
        //Serial.println("Cycle Complete");
      }
    }
  }
  fill_solid(leds, ledcnt, CRGB(prevR, prevG , prevB));
}

void setBlinking( CRGB leds[], int ledcnt, long r, long g, long b, bool lead, int cycleCount, bool rev) {
  // same variable reason as setBreath, except rev is added to determine the direction of blinking

  if (blinkCount != ledcnt - 1) {
    if (lead == true) {
      EVERY_N_MILLISECONDS(blinkCycleTime / ledcnt) {
        blinkCount = blinkCount + 1;
      }
    }
    for (int i = 0 ; i < blinkCount; i++) {
      if (rev == true) {
        leds[i] = CRGB(r, g, b);
      } else {
        leds[ledcnt - 1 - i] = CRGB(r, g, b);
      }
    }
  } else if (blinkCycleCount != cycleCount && blinkCount >= ledcnt - 1) {
    fill_solid(leds,  ledcnt, CRGB::Black);
    Serial.println("blinked");
    Serial.println(lead);
    if (lead == true) {
      blinkCount = 0;
      blinkCycleCount = blinkCycleCount + 1;
    }
  } else {
    animationComplete = true;
    blinkCycleCount = 1;
    blinkCount = 0;
    Serial.println("finished blinking");
  }



}

void drawText(const char* msg, int startX) {
  int xTracker = startX;
  for (int i = 0; msg[i] != '\0'; i++) {
    char c = msg[i];
    if (c >= 'A' && c <= 'Z') {
      uint8_t fontIdx = c - 'A';
      for (int col = 0; col < 5; col++) {
        int xPos = xTracker + col;
        // Only draw pixels if they fall within the 8-pixel window
        if (xPos >= 0 && xPos < MATRIX_WIDTH) {
          uint8_t colData = font5x7[fontIdx][col];
          for (int row = 0; row < 8; row++) {
            if (colData & (1 << row)) {
              matrix[XY(xPos, row)] = CRGB::Cyan; // Set color here
            }
          }
        }
      }
    }
    xTracker += 6; // Move pointer for next char (5 width + 1 space)
  }
}

void setup() {
  FastLED.addLeds<WS2812, PIN_RFRONT,  GRB>(frontR,  RFRONT);
  FastLED.addLeds<WS2812, PIN_LFRONT,  GRB>(frontL, LFRONT);
  FastLED.addLeds<WS2812, PIN_RBACK,   GRB>(backR,   RBACK);
  FastLED.addLeds<WS2812, PIN_LBACK,   GRB>(backL,  LBACK);
  FastLED.addLeds<WS2812, PIN_MATRIX, GRB>(matrix, MATRIX_LEDS);

  FastLED.setBrightness(60);

  Serial.begin(115200);
  delay(1000);
  Serial.println("AMR LED + MATRIX READY");

  fill_solid(frontR,  RFRONT, CRGB::Black);
  fill_solid(frontL, LFRONT, CRGB::Black);
  fill_solid(backR,   RBACK,  CRGB::Black);
  fill_solid(backL,  LBACK,  CRGB::Black);
  fill_solid(matrix, MATRIX_LEDS, CRGB::Red);

  //animation variable init
  breathCycleTime = 600;
  breathSteps = 20;
}

/* =========================
   LOOP
   ========================= */
void loop() {

  handleSerial();
  //  fill_solid(frontR,  RFRONT, CRGB::Black);
  //  fill_solid(frontL, LFRONT, CRGB::Black);
  //  fill_solid(backR,   RBACK,  CRGB::Black);
  //  fill_solid(backL,  LBACK,  CRGB::Black);
  //  fill_solid(matrix, MATRIX_LEDS, CRGB::Black);

  switch (currentState)  {

    case STANDBY:
        
        setBreathing(frontR, RFRONT, 153, 153, 153, true, 2 );
        setBreathing(frontL, LFRONT, 153, 153, 153, false, 2 );
        setBreathing(backR, RBACK, 153, 153, 153, false, 2 );
        setBreathing(backL, LBACK, 153, 153, 153, false, 2 );
        break;
    case MOVING:

      break;
    case RESPONSE:
      
      if (animationComplete == false) {
        setBreathing(frontR, RFRONT, 153, 153, 153, true, 3 );
        setBreathing(frontL, LFRONT, 153, 153, 153, false, 3 );
        setBreathing(backR, RBACK, 153, 153, 153, false, 3 );
        setBreathing(backL, LBACK, 153, 153, 153, false, 3 );
      } else {
        fill_solid(frontR,  RFRONT, CRGB(153, 153, 153));
        fill_solid(frontL, LFRONT, CRGB(153, 153, 153));
        fill_solid(backR,   RBACK,  CRGB(153, 153, 153));
        fill_solid(backL,  LBACK,  CRGB(153, 153, 153));
        animationComplete = false;
        currentState = NEUTRAL;
      }
      break;
    case TURN_LEFT:
      if (initialBlinkCall == true) {
        fill_solid(frontR,  RFRONT, CRGB::Black);
        fill_solid(frontL, LFRONT, CRGB::Black);
        fill_solid(backR,   RBACK,  CRGB::Black);
        fill_solid(backL,  LBACK,  CRGB::Black);
        FastLED.show();
        initialBlinkCall = false;
      }
      if (animationComplete == false) {
        fill_solid(frontL, LFRONT, CRGB(153, 153, 153));
        fill_solid(backL,  LBACK,  CRGB(153, 153, 153));
        setBlinking(frontR, RFRONT, 153, 153, 153 , true, 2, true);
        setBlinking(backR, RBACK, 153, 153, 153, false,  2, false);
      } else {
        fill_solid(frontR,  RFRONT, CRGB(153, 153, 153));
        fill_solid(frontL, LFRONT, CRGB(153, 153, 153));
        fill_solid(backR,   RBACK,  CRGB(153, 153, 153));
        fill_solid(backL,  LBACK,  CRGB(153, 153, 153));
        animationComplete = false;
        currentState = NEUTRAL;
        initialBlinkCall = true;
      }
      break;
    case TURN_RIGHT:
      //if (blinkState) fill_solid(frontL, LFRONT, CRGB::Blue);
      if (initialBlinkCall == true) {
        fill_solid(frontR,  RFRONT, CRGB::Black);
        fill_solid(frontL, LFRONT, CRGB::Black);
        fill_solid(backR,   RBACK,  CRGB::Black);
        fill_solid(backL,  LBACK,  CRGB::Black);
        FastLED.show();
        initialBlinkCall = false;
      }
      if (animationComplete == false) {
        fill_solid(frontR, LFRONT, CRGB(153, 153, 153));
        fill_solid(backR,  LBACK,  CRGB(153, 153, 153));
        setBlinking(frontL, RFRONT, 153, 153, 153 , true, 2, true);
        setBlinking(backL, RBACK, 153, 153, 153, false,  2, false);
      } else {
        fill_solid(frontR,  RFRONT, CRGB(153, 153, 153));
        fill_solid(frontL, LFRONT, CRGB(153, 153, 153));
        fill_solid(backR,   RBACK,  CRGB(153, 153, 153));
        fill_solid(backL,  LBACK,  CRGB(153, 153, 153));
        animationComplete = false;
        currentState = NEUTRAL;
        initialBlinkCall = true;
      }
      break;
    case GIVEWAY:

      break;
    case ARRIVE: //Show solid color blue at 60% brightness
      //      Serial.println("arrived");
      if (animationComplete == false) {
        //        Serial.println("color changing");
        setBreathing(frontR, RFRONT, 45, 153, 146, true, 3 );
        setBreathing(frontL, LFRONT, 45, 153, 146, false, 3 );
        setBreathing(backR, RBACK, 45, 153, 146, false, 3 );
        setBreathing(backL, LBACK, 45, 153, 146, false, 3 );
      } else {
        fill_solid(frontR,  RFRONT, CRGB(45, 153, 146));
        fill_solid(frontL, LFRONT, CRGB(45, 153, 146));
        fill_solid(backR,   RBACK,  CRGB(45, 153, 146));
        fill_solid(backL,  LBACK,  CRGB(45, 153, 146));
        animationComplete = false;
        currentState = NEUTRAL;
      }
      break;

  }
  if (printing == true) {

    EVERY_N_MILLISECONDS(120) {
      Serial.println(scrollOffset);
      Serial.println(displaytxt);
      scrollOffset++;
      fill_solid(matrix, MATRIX_LEDS, CRGB::Black);
      drawText(displaytxt.c_str(), scrollOffset);
      if (scrollOffset > 8) {
        //scrollOffset = MATRIX_WIDTH; //reset message scroll origin
        printing = false;
      }
    }
  }

  FastLED.show();
  delay(10);
}