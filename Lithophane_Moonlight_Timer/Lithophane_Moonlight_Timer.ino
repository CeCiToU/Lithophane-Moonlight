/*
  Lithophane Moonlight
  --------------------
  GitHub Repo: https://github.com/CeCiToU/Lithophane-Moonlight

  Description:
  This Arduino project controls a single LED (or a NeoPixel) to simulate different
  lighting modes for a moon lamp based on lithophane art. The lamp features
  multiple modes including white, rainbow, fire flicker, and several color modes.
  It also includes a 3-hour timer with a smooth fade-out effect. 
  The lamp can be restarted manually using an external switch, which resets the timer.

  Features:
  - Multiple lighting modes controlled via a potentiometer
  - Adjustable brightness via potentiometer
  - Fire flicker and rainbow animations
  - 3-hour timer with smooth 10-second fade-out
  - Only one LED is controlled (index 0), but code can be adapted for more LEDs
*/

#include <Adafruit_NeoPixel.h>

// --- CONFIGURATION ---
// Pin connected to NeoPixel data line
#define LED_PIN             6       
// Total number of LEDs (1 in this setup)
#define NUM_LEDS            1       
// Index of LED to control (0-based)
#define ACTIVE_LED          0       

// Analog pins for potentiometers
#define POT_MODE_PIN        A0      // Mode selector
#define POT_BRIGHTNESS_PIN  A1      // Brightness control

// Initialize NeoPixel strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// --- MODES ---
// Enum for different lighting modes
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

int currentMode = 0;        // Current selected mode
int brightness = 128;       // Current brightness
int previousBrightness = 25; // For fire flicker effect

// --- TIMER VARIABLES ---
unsigned long startTime;       // Timestamp when lamp was turned on
bool fading = false;           // Flag to indicate if fading is active
unsigned long fadeStartTime;   // Timestamp when fading started
const unsigned long RUN_TIME = 3UL * 60UL * 60UL * 1000UL; // 3 hours in milliseconds
const unsigned long FADE_TIME = 10000; // Fade-out duration (10 seconds)

void setup() {
  strip.begin();                  // Initialize NeoPixel library
  strip.setBrightness(brightness); 
  strip.show();                   // Turn off LED initially
  randomSeed(analogRead(2));      // Seed random generator for fire mode

  startTime = millis();           // Start the 3-hour timer
}

void loop() {
  unsigned long now = millis();   // Current time in milliseconds

  // --- TIMER CHECK ---
  // Start fading after RUN_TIME has passed
  if (!fading && now - startTime >= RUN_TIME) {
    fading = true;
    fadeStartTime = now;
  }

  // --- HANDLE FADE-OUT ---
  if (fading) {
    unsigned long elapsed = now - fadeStartTime;
    if (elapsed < FADE_TIME) {
      // Gradually reduce brightness
      int fadeBrightness = map(elapsed, 0, FADE_TIME, brightness, 0);
      strip.setBrightness(fadeBrightness);
      updateColor(); // Reapply current mode with new brightness
    } else {
      // Ensure LED is fully off
      strip.setBrightness(0);
      updateColor();
    }
    return; // Stop normal updates once fading
  }

  // --- READ POTENTIOMETERS ---
  int potBrightness = analogRead(POT_BRIGHTNESS_PIN); 
  int potMode = analogRead(POT_MODE_PIN);

  // Map potentiometer values to brightness and mode index
  brightness = map(potBrightness, 0, 1023, 0, 255);
  currentMode = map(potMode, 0, 1023, 0, MODE_COUNT - 1);
  strip.setBrightness(brightness);

  // --- HANDLE CURRENT MODE ---
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

  delay(50); // Small delay to reduce CPU usage
}

// --- SUPPORTING FUNCTIONS ---

// Reapply current mode (used during fade)
void updateColor() {
  switch (currentMode) {
    case MODE_WHITE:   setColor(255, 255, 255); break;
    case MODE_RAINBOW: rainbowCycle(500); break;
    case MODE_FIRE:    fireFlicker(); break;
    default: break;
  }
}

// Set LED to a specific RGB color
void setColor(uint8_t r, uint8_t g, uint8_t b) {
  strip.setPixelColor(ACTIVE_LED, strip.Color(r, g, b));
  strip.show();
}

// Fire flicker effect
void fireFlicker() {
  int rangeMin = max(0, previousBrightness - 10);
  int rangeMax = min(50, previousBrightness + 10);
  int target = random(rangeMin, rangeMax + 1);

  int steps = 5; // Smooth interpolation steps
  for (int i = 1; i <= steps; i++) {
    int interp = map(i, 0, steps, previousBrightness, target);
    int r = map(interp, 0, 50, 0, 255);
    int g = map(interp, 0, 50, 0, 100);
    strip.setPixelColor(ACTIVE_LED, strip.Color(r, g, 0));
    strip.show();
    delay(20);
  }
  previousBrightness = target;
}

// Rainbow mode effect
void rainbowCycle(uint8_t wait) {
  static uint16_t j = 0;
  strip.setPixelColor(ACTIVE_LED, Wheel((j++) & 255));
  strip.show();
  delay(wait);
}

// Generate RGB values for rainbow effect
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
