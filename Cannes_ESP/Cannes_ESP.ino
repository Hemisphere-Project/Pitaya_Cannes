
// PINS SUR LA CARTE: 23 22 11 5 21 19
#include "WiFi.h"
#include "Ticker.h"

// LED STRIP
#include "FastLED.h"
#define LED_PIN     23
#define NUM_LEDS    750
#define BRIGHTNESS  255
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
CRGB ledsReversed[NUM_LEDS];

// WAVE CLOCK
Ticker waveclock;

// WAVE NET
#include "Wave.h"
Wave* wave_blue;
Wave* wave_ressac;

// FISH NET
#include "Fishnet.h"
Fishnet fishnet;

// ENCODER
#define ENCODER_SPEED_FACTOR 150
unsigned long lastEncoderRead = 0;

//
// ASYNC WAIT
//
#define N_TIMER 10
unsigned long timers[N_TIMER];
bool wait(int t, uint32_t ms) {
  if (t>=N_TIMER) return false;
  if (timers[t] == 0) {
    timers[t] = millis();
    return true;
  }
  else if ((millis()-timers[t]) < ms) 
    return true;
  
  else return false;
}
void waitreset() {
  for (int t=0; t<N_TIMER; t++) timers[t] = 0;
}

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

  // WAVES
  wave_blue   = new Wave( CRGB(0, 10, 40),    NUM_LEDS,     57, EASE_QUAD );     // create wave (color, size, speed, easePOW)
  wave_ressac = new Wave( CRGB(10, 139, 30),  NUM_LEDS*0.3, 75, EASE_QUATR );     // create wave (color, size, speed, easePOW)
  waitreset();
  waveclock.attach_ms(25, waveanimator );

  // WHITE FISH
  Fish* fish_white = new Fish(10, 170, 70, 140, 40, 230);     // create fish (size, speedMinimum, boid_cooling, trailer_cooling, spark_chance, spark_brightness)
  fishnet.add( fish_white );                            // add to fishnet
  fish_white->setColor( CRGB(255,255,255) );            // set WHITE
  fish_white->jump(50);                                 // Initial position

  // WHITE FISH 2
  Fish* fish_white2 = new Fish(10, 120, 70, 140, 40, 230);     // create fish (size, speedMinimum, boid_cooling, trailer_cooling, spark_chance, spark_brightness)
  fishnet.add( fish_white2 );
  fish_white2->setColor( CRGB(255,255,255) );      // set WHITE
  fish_white2->jump(650);                          // Initial position


  // WHITE FISH 3
  Fish* fish_white3 = new Fish(10, 100, 70, 140, 40, 230);                // create fish with default settings
  fishnet.add( fish_white3 );
  fish_white3->setColor( CRGB(255,255,255) );    // set RED
  fish_white3->jump(1250);                   // Initial position

  // WHITE FISH 4
  Fish* fish_white4 = new Fish(10, 70, 70, 140, 40, 230);                // create fish with default settings
  fishnet.add( fish_white4 );
  fish_white4->setColor( CRGB(255,255,255) );    // set RED
  fish_white4->jump(1500);      
  

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
  if (diff != 0) fishnet.speedAnim( diff );



  delay(15);  // this delay influences ENCODER speed factor && WAVE speed factor
}

//
//  ANIMATE WAVES
//
void waveanimator() {

  // GROWING 0->50%
  if (wave_ressac->progress() < 50) {
    if (wave_blue->progress() < 50)  wave_blue->animate();
    if (wave_blue->progress() > 35)   wave_ressac->animate();
  }
  
  // FULL COLOR PAUSE
  else if (wait(0,100));

  // DECAYING 50->100%
  else if (wave_blue->progress() < 100) {
    if (wave_ressac->progress() < 100) wave_ressac->animate();
    if (wave_ressac->progress() > 70) wave_blue->animate();
  }

  // BLACK PAUSE
  else if (wait(1,2000));

  // RESET 
  else {
    wave_ressac->reset();
    wave_blue->reset();
    waitreset();
  }

}


//
// DRAW (Core 0)
//
void draw() {
  FastLED.clear();
  wave_blue->draw(leds);
  wave_ressac->draw(leds);
  fishnet.draw(leds);
  FastLED.show();
  delay(1);
}

