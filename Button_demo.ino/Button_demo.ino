/* MakeFashion Kit Button demo
 * -------------------------------------------
 *  
 * Requires:
 * ---------
 *  MakeFashion controller board
 *  SeeedStudio Button ( http://wiki.seeedstudio.com/Grove-Button/ )
 *  FastLED-compatible LEDs ( https://github.com/FastLED/FastLED/wiki/Overview#chipsets )
 *  
 * Dependencies:
 * -------------
 *  FastLED ( Available via Arduino package manager )
 */

#include "FastLED.h"

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define PIN_LEDARRAY    6
#define PIN_BUTTON       3

#define LED_TYPE           WS2812B
#define COLOR_ORDER        GRB
#define NUM_LEDS           25
#define BRIGHTNESS         96
#define FRAMES_PER_SECOND  30
#define TOGGLE_INTERVAL    2000 // milliseconds

// function prototypes
void rainbow(CRGB *gArray, int gSize);
void rainbowWithGlitter(CRGB *gArray, int gSize);
void confetti(CRGB *gArray, int gSize);
void sinelon(CRGB *gArray, int gSize);
void sinelon_red(CRGB *gArray, int gSize);
void sinelon_blue(CRGB *gArray, int gSize);
void sinelon_redblue(CRGB *gArray, int gSize);
void juggle(CRGB *gArray, int gSize);
void bpm(CRGB *gArray, int gSize);
void juggle(CRGB *gArray, int gSize);
void juggle_red(CRGB *gArray, int gSize);
void juggle_blue(CRGB *gArray, int gSize);
void juggle_redblue(CRGB *gArray, int gSize);
void blinky(CRGB *gArray, int gSize);

typedef void (*SimplePatternList)(CRGB *, int);
SimplePatternList gPatterns[] = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, sinelon_red, \
                                  sinelon_blue, juggle_red, juggle_blue, sinelon_redblue, juggle_redblue };

CRGB leds_array[NUM_LEDS];
unsigned int gArray_PatternNumber = 0;
unsigned int gHue = 0; // rotating "base color" used by many of the patterns
unsigned int gBlinky = 0;
bool ledpower;
bool state_toggle;
unsigned int state_now;
unsigned int state_last;
unsigned long time_start;
unsigned long time_interval;

void setup() {
  delay(3000); // 3 second delay for recovery

  Serial.begin(9600);

  ledpower = true;
  state_toggle = false;
  state_now = LOW;
  state_last = LOW;
  time_start = millis();
  time_interval = 0;
  
  FastLED.addLeds<LED_TYPE, PIN_LEDARRAY, COLOR_ORDER>(leds_array, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

}

void loop()
{
  state_now = digitalRead(PIN_BUTTON);

  check_power_toggle();
  gPatterns[gArray_PatternNumber](leds_array, NUM_LEDS);
  
  if (ledpower) { FastLED.show(); }
  else { FastLED.clear(); }
 
  EVERY_N_MILLISECONDS( 20 ) { gHue++; }
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

void check_power_toggle() {
  if(state_now != state_last) {
    if( state_toggle) {
      state_toggle = false;
      nextPattern();
    } else {
      if( state_now ) {
        state_toggle = true;
        time_start = millis();
      }
    }
  } else {
    if(state_toggle) {
      time_interval = millis() - time_start;
      if(time_interval > TOGGLE_INTERVAL) {
        ledpower = not ledpower;
        state_toggle = false;
        time_start = millis();
      }
    }
  }
  state_last = state_now;
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gArray_PatternNumber = (gArray_PatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void rainbow(CRGB *gArray, int gSize)
{
  // FastLED's built-in rainbow generator
  
  fill_rainbow( gArray, gSize, gHue, 7);
}

void rainbowWithGlitter(CRGB *gArray, int gSize)
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow(gArray, gSize);
  addGlitter(gArray, gSize);
}

void addGlitter(CRGB *gArray, int gSize)
{
  if ( random8() < 80) {
    gArray[ random16(gSize) ] += CRGB::White;
  }
}

void confetti(CRGB *gArray, int gSize)
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( gArray, gSize, 10);
  int pos = random16(gSize);
  gArray[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon(CRGB *gArray, int gSize)
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( gArray, gSize, 20);
  int pos = beatsin16(13, 0, gSize - 1);
  gArray[pos] += CHSV( gHue, 255, 192);
}

void sinelon_redblue(CRGB *gArray, int gSize)
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( gArray, gSize, 20);
  int pos1 = beatsin16(13, 0, gSize - 1);
  int pos2 = gSize - pos1;
  gArray[pos1] += CHSV( 0, 255, 192);
  gArray[pos2] += CHSV( 160, 255, 192);
}

void sinelon_red(CRGB *gArray, int gSize)
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( gArray, gSize, 20);
  int pos = beatsin16(13, 0, gSize - 1);
  gArray[pos] += CHSV( 0, 255, 192);
}

void sinelon_blue(CRGB *gArray, int gSize)
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( gArray, gSize, 20);
  int pos = beatsin16(13, 0, gSize - 1);
  gArray[pos] += CHSV(160, 255, 192);
}

void bpm(CRGB *gArray, int gSize)
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < gSize; i++) { //9948
    gArray[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle(CRGB *gArray, int gSize) {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( gArray, gSize, 20);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    gArray[beatsin16(i + 7, 0, gSize - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void juggle_red(CRGB *gArray, int gSize) {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( gArray, gSize, 20);
  for ( int i = 0; i < 8; i++) {
    gArray[beatsin16(i + 7, 0, gSize - 1)] |= CHSV(0, 255, 192);
  }
}

void juggle_blue(CRGB *gArray, int gSize) {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( gArray, gSize, 20);
  for ( int i = 0; i < 8; i++) {
    gArray[beatsin16(i + 7, 0, gSize - 1)] |= CHSV(160, 255, 192);
  }
}

void juggle_redblue(CRGB *gArray, int gSize) {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( gArray, gSize, 20);
  for ( int i = 0; i < 8; i++) {
    gArray[beatsin16(i + 7, 0, gSize - 1)] |= CHSV(0, 255, 192);
    gArray[gSize - beatsin16(i + 7, 0, gSize - 1)] |= CHSV(160, 255, 192);
  }
}

void blinky(CRGB *gArray, int gSize) {
  gBlinky = not gBlinky;
  CRGB color = CRGB::Red;
  if ( gBlinky ) {
    color = CRGB::Black;
  } else {
    color = CRGB::Red;
  }
  for ( int i = 0; i < gSize; i++ ) {
    gArray[i] = color;
  }
}



