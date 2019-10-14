
// PINS SUR LA CARTE: 23 22 11 5 21 19

// LED STRIP
#include "FastLED.h"
#define LED_PIN     23
#define NUM_LEDS    750
#define BRIGHTNESS  255
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];


void setup() {

  delay(2000);
  // LED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );
  //NeoPixel
  // pixels.begin();

  Serial.begin(115200);

}


unsigned long timeNow;

void loop() {

  FastLED.clear();

  // base blue
  for(int i=0; i<NUM_LEDS; i++) {
    leds[i] = CRGB(22, 0, 90);
  }
  
  int colorSize = 100;
  int colorSpace = 50;
  int spacer = colorSpace*6;

  // green
  for(int i=spacer; i<colorSize+spacer; i++) {
    leds[i] = CRGB(0, 120, 55);
  }

  spacer += colorSpace;

  // yellow
  for(int i=colorSize+spacer; i<colorSize*2+spacer; i++) {
    leds[i] = CRGB(120, 75, 0);
  }

  spacer += colorSpace;

  // red
  for(int i=colorSize*2+spacer; i<colorSize*3+spacer; i++) {
    leds[i] = CRGB(160, 40, 0);
  }

  // showtime
  FastLED.show();
  delay(1);
}
