
// PINS SUR LA CARTE: 23 22 11 5 21 19
#include "WiFi.h"

// LED STRIP
#include "FastLED.h"
#define LED_PIN     23
#define NUM_LEDS    1500
#define BRIGHTNESS  250
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

// FISH NET
#include "Fishnet.h"
Fishnet fishnet;

// ENCODER
#define ENCODER_SPEED_FACTOR 150
unsigned long lastEncoderRead = 0;

//
// SETUP
//
void setup() {

  Serial.begin(115200);

  // RADIO OFF
  WiFi.mode(WIFI_OFF);
  btStop();

  // CODER
  setupEncoder(21, 19); // (PIN1 / PIN2)

  // LED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );

  // WHITE FISH
  Fish* fish_white = new Fish(20, 30, 70, 140, 40, 230);     // create fish (size, speedMinimum, boid_cooling, trailer_cooling, spark_chance, spark_brightness)
  fishnet.add( fish_white );                            // add to fishnet
  fish_white->setColor( CRGB(255,255,255) );            // set WHITE
  fish_white->jump(50);                                 // Initial position

  // WHITE FISH 2
  Fish* fish_white2 = new Fish(40, 30, 70, 140, 40, 230);     // create fish (size, speedMinimum, boid_cooling, trailer_cooling, spark_chance, spark_brightness)
  fishnet.add( fish_white2 );
  fish_white2->setColor( CRGB(255,255,255) );      // set WHITE
  fish_white2->jump(650);                          // Initial position


  // RED FISH 
  Fish* fish_white3 = new Fish(15);                // create fish with default settings
  fishnet.add( fish_white3 );
  fish_white3->setColor( CRGB(255,255,255) );    // set RED
  fish_white3->jump(1250);                   // Initial position
  

  // DRAW TASK
  xTaskCreatePinnedToCore(
                    [](void * pvParameters){ while(true) draw(); },   /* Function to implement the task */
                    "drawTask", /* Name of the task */
                    10000,      /* Stack size in words */
                    NULL,       /* Task input parameter */
                    1,          /* Priority of the task */
                    NULL,       /* Task handle. */
                    0);  /* Core where the task should run */

}

//
// LOOP (Core 1)
//
void loop() {

  // GET SPEED FROM ENCODER:
  int lapse = millis() - lastEncoderRead;
  lastEncoderRead = millis();
  if (lapse == 0) lapse = 1;
  int diff = getEncoderDiff() * ENCODER_SPEED_FACTOR/lapse;
  if (diff != 0) Serial.println(diff);

  // Move FISHNET with encoder
  if (diff != 0) fishnet.speedAim( diff );

  // WAVE progress
  wave_progress();

  delay(15);  // this delay influences ENCODER speed factor && WAVE speed factor
}

//
// DRAW (Core 0)
//
void draw() {
  FastLED.clear();
  wave_draw(leds);
  fishnet.draw(leds);
  FastLED.show();
  delay(1);
}

