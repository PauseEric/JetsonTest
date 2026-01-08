#include <FastLED.h>

// --- Configuration ---
#define LED_PIN     26      // Pin as requested
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define BRIGHTNESS  30

#define kMatrixWidth  8     // Width as requested
#define kMatrixHeight 8
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)

CRGB leds[NUM_LEDS];

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

// --- Scrolling Variables ---
const char* message = "TESTING";
int textWidth = strlen(message) * 6; // Width of text + spacing
int scrollOffset = 0;                // 0 means it starts at the first column (the T)

// --- Serpentine (Zigzag) Mapping ---
uint16_t XY(uint8_t x, uint8_t y) {
  if (x >= kMatrixWidth || y >= kMatrixHeight) return NUM_LEDS;
  if (y & 0x01) { // Odd rows are reversed for serpentine panels
    return (y * kMatrixWidth) + ((kMatrixWidth - 1) - x);
  } else { // Even rows are normal
    return (y * kMatrixWidth) + x;
  }
}

void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
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
        if (xPos >= 0 && xPos < kMatrixWidth) {
          uint8_t colData = font5x7[fontIdx][col];
          for (int row = 0; row < 8; row++) {
            if (colData & (1 << row)) {
              leds[XY(xPos, row)] = CRGB::Cyan; // Set color here
            }
          }
        }
      }
    }
    xTracker += 6; // Move pointer for next char (5 width + 1 space)
  }
}

void loop() {
  FastLED.clear();
  
  // Draw the current frame
  drawText(message, scrollOffset);
  FastLED.show();

  // Decrease offset to move text from Right to Left
  scrollOffset++;

  // Reset logic: once text scrolls completely off the left edge (-textWidth)
  // move it back to the right edge (kMatrixWidth) to start over
  if (scrollOffset < -textWidth) {
    scrollOffset = kMatrixWidth;
  }

  delay(120); // Control scrolling speed
}