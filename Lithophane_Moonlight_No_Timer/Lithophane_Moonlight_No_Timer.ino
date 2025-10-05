/*
  Lithophane Moonlight – Multi-LED Variant
  ----------------------------------------
  GitHub Repo: https://github.com/CeCiToU/Lithophane-Moonlight

  Description:
  This Arduino project controls a single LED on a strip of 120 NeoPixels 
  to simulate different lighting modes for a moon lamp based on lithophane art. 
  The lamp supports multiple modes including white, rainbow, fire flicker, and several color modes.
  The brightness and mode are controlled via potentiometers. Serial output is included
  for debugging purposes.

  Features:
  - Adjustable brightness using a potentiometer
  - Mode selection via potentiometer
  - Fire flicker effect with smooth transitions
  - Rainbow mode effect
  - Debugging output via Serial monitor
  - Configured to control one LED (index 0), but can be adapted to control multiple LEDs
*/

#include <Adafruit_NeoPixel.h>

// --- CONFIGURATION ---
// NeoPixel data pin
#define PIN                 6
// Total number of LEDs on the strip
#define NUM_LEDS            1
// Minimum and maximum brightness
#define BRIGHTNESS_MIN      0
#define BRIGHTNESS_MAX      255
// LED index to control (0-based)
#define ACTIVE_LED          0  

// Potentiometer pins
#define POT_MODE_PIN        A0  // Mode selector
#define POT_BRIGHTNESS_PIN  A1  // Brightness control

// Initialize the NeoPixel strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// --- MODES ---
enum Mode {
  MODE_FIRE,
  MODE_RAINBOW,
  MODE_WHITE,
  MODE_RED,
  MODE_ORANGE,
  MODE_YELLOW,
  MODE_LIGHT_GREEN,
  MODE_GREEN,
  MODE_CYAN,
  MODE_BLUE,
  MODE_VIOLET,
  MODE_MAGENTA,
  MODE_INDIGO,
  MODE_EXTRA,
  MODE_COUNT
};

// Names for Serial output
const char* modeNames[MODE_COUNT] = {
  "FIRE", "RAINBOW", "WHITE", "RED", "ORANGE", "YELLOW", "LIGHT_GREEN", "GREEN",
  "CYAN", "BLUE", "VIOLET", "MAGENTA", "INDIGO", "EXTRA"
};

// Current mode and brightness
int currentMode = 0;
int brightness = BRIGHTNESS_MIN;

// Previous brightness for fire flicker effect
int previousBrightness = 25;

void setup() {
  strip.begin();                 // Initialize NeoPixel library
  strip.setBrightness(brightness); 
  strip.show();                  // Ensure LEDs are off initially
  randomSeed(analogRead(2));    // Seed for fire flicker randomness
  Serial.begin(9600);            // Start Serial for debugging
}

void loop() {
  // --- READ POTENTIOMETERS ---
  int potBrightness = analogRead(POT_BRIGHTNESS_PIN); 
  int potMode = analogRead(POT_MODE_PIN);

  // Map potentiometer readings to brightness and mode index
  brightness = map(potBrightness, 0, 1023, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
  currentMode = map(potMode, 0, 1023, 0, MODE_COUNT - 1);

  strip.setBrightness(brightness);

  // --- DEBUGGING OUTPUT ---
  Serial.print("Brightness: ");
  Serial.print(brightness);
  Serial.print(" | Mode: ");
  Serial.println(modeNames[currentMode]);

  // --- HANDLE MODES ---
  switch (currentMode) {
    case MODE_WHITE:        setColor(255, 255, 255); break;
    case MODE_RAINBOW:      rainbowCycle(500); break;
    case MODE_FIRE:         fireFlicker(); break;
    case MODE_RED:          setColor(255, 0, 0); break;
    case MODE_ORANGE:       setColor(255, 165, 0); break;
    case MODE_YELLOW:       setColor(255, 255, 0); break;
    case MODE_LIGHT_GREEN:  setColor(64, 255, 64); break;
    case MODE_GREEN:        setColor(0, 255, 0); break;
    case MODE_CYAN:         setColor(0, 255, 255); break;
    case MODE_BLUE:         setColor(0, 0, 255); break;
    case MODE_VIOLET:       setColor(148, 0, 211); break;
    case MODE_MAGENTA:      setColor(255, 0, 255); break;
    case MODE_INDIGO:       setColor(75, 0, 130); break;
    case MODE_EXTRA:        setColor(128, 0, 128); break;
  }

  delay(50); // Small delay for stability
}

// --- SUPPORTING FUNCTIONS ---

// Set a single LED to a specific color
void setColor(uint8_t r, uint8_t g, uint8_t b) {
  strip.clear(); // Clear all LEDs
  strip.setPixelColor(ACTIVE_LED, strip.Color(r, g, b));
  strip.show();
}

// Fire flicker effect
void fireFlicker() {
  int rangeMin = max(0, previousBrightness - 10);
  int rangeMax = min(50, previousBrightness + 10);
  int targetBrightness = random(rangeMin, rangeMax + 1);

  int steps = 5; // Smooth transition steps
  for (int i = 1; i <= steps; i++) {
    int intermediateBrightness = map(i, 0, steps, previousBrightness, targetBrightness);
    setFireColor(ACTIVE_LED, intermediateBrightness);
    strip.show();
    delay(20);
  }
  previousBrightness = targetBrightness;
}

// Set fire color for a single LED
void setFireColor(int pixel, int brightness) {
  int r = map(brightness, 0, 50, 0, 255);
  int g = map(brightness, 0, 50, 0, 100);
  int b = 0;
  strip.setPixelColor(pixel, strip.Color(r, g, b));
}

// Rainbow effect for a single LED
void rainbowCycle(uint8_t wait) {
  static uint16_t j = 0;
  strip.setPixelColor(ACTIVE_LED, Wheel((j++) & 255));
  strip.show();
  delay(wait);
}

// Generate RGB color for rainbow effect
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
