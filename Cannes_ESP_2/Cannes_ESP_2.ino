
// PINS SUR LA CARTE: 23 22 11 5 21 19

// LED STRIP
#include "FastLED.h"
#define LED_PIN     23
#define NUM_LEDS    185
#define BRIGHTNESS  200
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

// FISH
#include "Fish.h"
Fish fish_white(20);
Fish fish_red(10);

// TICKERS
#include "Ticker.h"
Ticker tickerRed_move;

// ENCODER
long lastEncoderRead = 0;

//
// SETUP
//
void setup() {

  Serial.begin(115200);

  // CODER
  setupEncoder(21, 19); // (PIN1 / PIN2)

  // LED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );

  // WHITE FISH
  fish_white.color( CRGB(255,255,255) );    // set RED
  fish_white.jump(50);                  // Initial position

  // RED FISH 
  fish_red.color( CRGB(255,0,0) );    // set RED
  fish_red.jump(50);                  // Initial position
  
  // Automove RED FISH
  tickerRed_move.attach_ms(30, [](){ 
    fish_red.move_up(); 
  });

  

}

//
// LOOP
//
void loop() {

  // **FAKE** Encoder change
  // int now = (millis() / 1000);
  // if ( now%6 < 3 && millis()%4 == 1  && random8(10)<6) lastEncoderRead += 1;

  // Move WHITE FISH with encoder
  int encodeMove = getEncoder() - lastEncoderRead;
  lastEncoderRead = getEncoder();
  while (encodeMove < 0) {
    encodeMove += 1;
    fish_white.move_down();
  }
  while (encodeMove > 0) {
    encodeMove -= 1;
    fish_white.move_up();
  }

  // Draw
  FastLED.clear();
  // wave(leds);
  fish_red.draw(leds);
  fish_white.draw(leds);
  FastLED.show();

  delay(1);
}
