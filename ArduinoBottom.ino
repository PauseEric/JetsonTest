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
#define PIN_LFRONT   23
#define PIN_RBACK    24
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

/* =========================
   STATE
   ========================= */
enum Direction { GIVEWAY, MOVING, STOP, TURN_LEFT, TURN_RIGHT, ARRIVE };
Direction currentState = GIVEWAY;

/* =========================
   TIMERS
   ========================= */
unsigned long lastBlink  = 0;
unsigned long lastBreath = 0;
unsigned long lastScroll = 0;

bool blinkState = false;
uint8_t breathBrightness = 60;
int breathDelta = 2;

int scrollX = MATRIX_WIDTH;

//Breathing Timers

const uint8_t breathingCSV[] PROGMEM = {
  0, 10, 30, 70, 140, 210, 255, 255, // Inhale
  210, 140, 70, 30, 10, 0, 0         // Exhale & Pause
};
const uint8_t tableSize = sizeof(breathingCSV) / sizeof(uint8_t);
uint8_t temp = 0;



/* =========================
   MOVING DECLARATIONS
   ========================= */
void handleSerial();
void processCommand(char* cmd);
void setMatrixText(const char* text);
void updateBlink();
void updateBreathing();
int  XY(int x, int y);

/* =========================
   FONT (A–Z, 5x7)
   ========================= */
const uint8_t font5x7[][5] = {
  {0x7E, 0x11, 0x11, 0x7E, 0x00}, {0x7F, 0x49, 0x49, 0x36, 0x00},
  {0x3E, 0x41, 0x41, 0x22, 0x00}, {0x7F, 0x41, 0x41, 0x3E, 0x00},
  {0x7F, 0x49, 0x49, 0x41, 0x00}, {0x7F, 0x09, 0x09, 0x01, 0x00},
  {0x3E, 0x41, 0x51, 0x32, 0x00}, {0x7F, 0x08, 0x08, 0x7F, 0x00},
  {0x00, 0x41, 0x7F, 0x41, 0x00}, {0x20, 0x40, 0x41, 0x3F, 0x00},
  {0x7F, 0x08, 0x14, 0x63, 0x00}, {0x7F, 0x40, 0x40, 0x40, 0x00},
  {0x7F, 0x02, 0x04, 0x02, 0x7F}, {0x7F, 0x04, 0x08, 0x7F, 0x00},
  {0x3E, 0x41, 0x41, 0x3E, 0x00}, {0x7F, 0x09, 0x09, 0x06, 0x00},
  {0x3E, 0x41, 0x61, 0x7E, 0x00}, {0x7F, 0x09, 0x19, 0x66, 0x00},
  {0x26, 0x49, 0x49, 0x32, 0x00}, {0x01, 0x01, 0x7F, 0x01, 0x01},
  {0x3F, 0x40, 0x40, 0x3F, 0x00}, {0x1F, 0x20, 0x40, 0x20, 0x1F},
  {0x3F, 0x40, 0x38, 0x40, 0x3F}, {0x63, 0x14, 0x08, 0x14, 0x63},
  {0x07, 0x08, 0x70, 0x08, 0x07}, {0x61, 0x51, 0x49, 0x45, 0x43},
};

/* =========================
   MATRIX XY MAPPING (SERPENTINE)
   ========================= */
int XY(int x, int y) {
  if (x < 0 || x >= MATRIX_WIDTH)  return -1;
  if (y < 0 || y >= MATRIX_HEIGHT) return -1;

  if (y & 1)
    return y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
  else
    return y * MATRIX_WIDTH + x;
}

/* =========================
   NON-BLOCKING SERIAL
   ========================= */
void handleSerial() {
  if (Serial.available() > 0) {
    cmd = Serial.readStringUntil('\n');
    if (cmd.length() > 0) {
      if (cmd == "move") {
        currentState = MOVING;
      } else if (cmd == "stop") {
        currentState = STOP;
      } else if (cmd == "turning_left") {
        currentState = TURN_LEFT;
      } else if (cmd == "turning_right") {
        currentState = TURN_RIGHT;
      } else if (cmd == "giveway") {
        currentState = GIVEWAY;
      } else if (cmd == "arrived") {
        currentState = ARRIVE;
      } else if (cmd == "amr_number") {

      } else if (cmd == "pharmacy") {

      } else if (cmd == "test") {

      }

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

void setMatrixText(const char* text) {
  uint8_t i = 0;
  while (text[i] && i < MATRIX_TEXT_MAX - 1) {
    char c = text[i];
    if (c >= 'a' && c <= 'z') c -= 32;
    matrixText[i++] = c;
  }
  matrixText[i] = '\0';
  scrollX = MATRIX_WIDTH;
}

/* =========================
   EFFECTS
   ========================= */
void updateBlink() {
  if (millis() - lastBlink >= 400) {
    lastBlink = millis();
    blinkState = !blinkState;
  }
}

void updateBreathing() {
  if (millis() - lastBreath >= 30) {
    lastBreath = millis();
    breathBrightness += breathDelta;
    if (breathBrightness >= 120 || breathBrightness <= 10)
      breathDelta = -breathDelta;
  }
}

/* =========================
   SETUP
   ========================= */
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
  fill_solid(matrix, MATRIX_LEDS, CRGB::Black);

}

/* =========================
   LOOP
   ========================= */
void loop() {

  handleSerial();
  updateBlink();
  updateBreathing();
  //
  //  fill_solid(frontR,  RFRONT, CRGB::Black);
  //  fill_solid(frontL, LFRONT, CRGB::Black);
  //  fill_solid(backR,   RBACK,  CRGB::Black);
  //  fill_solid(backL,  LBACK,  CRGB::Black);
  //  fill_solid(matrix, MATRIX_LEDS, CRGB::Black);

  switch (currentState) {
    case MOVING:
      fill_solid(frontR,  RFRONT, CRGB(255, 255, 255));
      fill_solid(frontL, LFRONT, CRGB(255, 255, 255));
      fill_solid(backR,   RBACK,  CRGB(153, 153, 153));
      fill_solid(backL,  LBACK,  CRGB(153, 153, 153));
      break;
    case STOP:
      fill_solid(frontR,  RFRONT, CRGB::Black);
      fill_solid(frontL, LFRONT, CRGB::Black);
      fill_solid(backR,   RBACK,  CRGB::Black);
      fill_solid(backL,  LBACK,  CRGB::Black);
      fill_solid(matrix, MATRIX_LEDS, CRGB::Black);
      break;
    case TURN_LEFT:
      //if (blinkState) fill_solid(backL, LBACK, CRGB::Blue);
      fill_solid(frontL, LFRONT, CHSV(100, 0, breathBrightness));
      fill_solid(backL,  LBACK,  CHSV(100, 0, breathBrightness));
      fill_solid(frontR,  RFRONT, CRGB(153, 153, 153));
      fill_solid(backR,   RBACK,  CRGB(153, 153, 153));
      break;
    case TURN_RIGHT:
      //if (blinkState) fill_solid(frontL, LFRONT, CRGB::Blue);
      temp = CSVBreathing();
      fill_solid(frontL, LFRONT, CRGB(153, 153, 153));
      fill_solid(backL,  LBACK,  CRGB(153, 153, 153));
      fill_solid(frontR,  RFRONT, CHSV(100, 0, temp));
      fill_solid(backR,   RBACK,  CHSV(100, 0, temp));

      break;
    case GIVEWAY:
      fill_solid(frontR,  RFRONT, CHSV(0, 255, breathBrightness));
      fill_solid(frontL, LFRONT, CHSV(0, 255, breathBrightness));
      fill_solid(backR,   RBACK,  CHSV(0, 255, breathBrightness));
      fill_solid(backL,  LBACK,  CHSV(0, 255, breathBrightness));
      break;
    case ARRIVE: //Show solid color blue at 60% brightness
      fill_solid(frontR,  RFRONT, CRGB(0, 195, 181));
      fill_solid(frontL, LFRONT, CRGB(0, 195, 181));
      fill_solid(backR,   RBACK,  CRGB(0, 195, 181));
      fill_solid(backL,  LBACK,  CRGB(0, 195, 181));
  }

  /* ===== MATRIX SCROLL ===== */
  if (millis() - lastScroll >= 120) {
    lastScroll = millis();
    int x = scrollX;

    for (uint8_t i = 0; matrixText[i]; i++) {
      char c = matrixText[i];
      if (c < 'A' || c > 'Z') continue;

      const uint8_t* ch = font5x7[c - 'A'];
      for (int cx = 0; cx < 5; cx++) {
        for (int cy = 0; cy < 7; cy++) {
          if (ch[cx] & (1 << cy)) {
            int px = x + cx;
            int idx = XY(px, cy);
            if (idx >= 0) matrix[idx] = CRGB::Green;
          }
        }
      }
      x += 6;
    }

    scrollX--;
    int textWidth = strlen(matrixText) * 6;
    if (scrollX < -textWidth) scrollX = MATRIX_WIDTH;
  }

  FastLED.show();
  delay(500);
}


uint8_t CSVBreathing() {
  static uint8_t index = 0;
  //static uint8_t hue = 160; // Blue hue

  // 2. Control update speed (e.g., 60ms per data point)
  EVERY_N_MILLISECONDS(60) {
    // 3. Read the 'Value' from the CSV table
    uint8_t val = pgm_read_byte(&(breathingCSV[index]));
    index = (index + 1) % tableSize;
    // 4. Apply using CHSV (Hue, Saturation, Value)
    // fill_solid converts CHSV to CRGB automatically
    //fill_solid(leds, NUM_LEDS, CHSV(hue, 255, val));

    // FastLED.show();

    // Move to next step in the CSV data
    return (val);
  }
}