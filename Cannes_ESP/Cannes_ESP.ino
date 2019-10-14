/* 
   ___ _ _ 
  / _ (_) |_ __ _ _   _  __ _( )__    / __(_)___| |__   ___ _ __ _ __ ___   __ _ _ __  
 / /_)/ | __/ _` | | | |/ _` |/ __|  / _\ | / __| '_ \ / _ \ '__| '_ ` _ \ / _` | '_ \ 
/ ___/| | || (_| | |_| | (_| |\__ \ / /   | \__ \ | | |  __/ |  | | | | | | (_| | | | |
\/    |_|\__\__,_|\__, |\__,_||___/ \/    |_|___/_| |_|\___|_|  |_| |_| |_|\__,_|_| |_|
                  |___/                           
*/                                     

// PINS SUR LA CARTE: 23 22 11 5 21 19
#include "WiFi.h"
#include "Ticker.h"
#include "FastLED.h"

// *******************************************
//

// COLORS
CRGB colorBLUE  = CRGB(22, 0, 90);    // couleur vague bleue
CRGB colorGREEN = CRGB(0, 120, 55);   // couleur ressac vert
CRGB colorYELW  = CRGB(120, 75, 0);   // couleur ressac jaune
CRGB colorRED   = CRGB(160, 40,  0);  // couleur ressac orange

// PAUSE BLACK
int pauseBLACK = 5000;                // pause au noir en millisecondes

// FISH MODE
#define WAVE_COUNT_CHANGE 2           // nombre de vague avant changement de mode

//ENCODER
#define ENCODER_SPEED_FACTOR 150      // sensibilité encoder
bool encoderReverse = true;           // inversion encoder [true | false]

//
// *******************************************

// LED STRIP
#define LED_PIN     23
#define NUM_LEDS    750
#define BRIGHTNESS  255
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
CRGB ledsReversed[NUM_LEDS];

// WAVE NET
#include "Wave.h"
Wave* wave_blue;
Wave* wave_ressac;
Wave* ressac_green;
Wave* ressac_yellow;
Wave* ressac_red;

// WAVE CLOCK
Ticker waveclock;

// FISH NET
#include "Fish.h"
#define MAX_FISH 25
Fish fishnet[MAX_FISH];

// MODE
int wave_count = 0;
int FISHMODE = 1;

// ENCODER
unsigned long lastEncoderRead = 0;


//
// ASYNC WAIT
//
#define N_TIMER 10
unsigned long timers[N_TIMER];
bool wait(int t, uint32_t ms, bool autoreset) 
{
  if (t>=N_TIMER) return false;
  if (timers[t] == 0) {
    timers[t] = millis();
    return true; // start waiting
  }
  else if ((millis()-timers[t]) < ms) {
    return true;  // still waiting
  }
  
  else {
    if (autoreset) timers[t] = 0; //re-arm 
    return false; // wait is over !
  }
}
bool wait(int t, uint32_t ms) { return wait(t, ms, true); }

void wavereset() {
  timers[0] = 0;
  wave_blue->reset();
  ressac_green->reset();
  ressac_yellow->reset();
  ressac_red->reset();

  int r = random(3);
  if (r == 0) wave_ressac = ressac_green;
  if (r == 1) wave_ressac = ressac_yellow;
  if (r == 2) wave_ressac = ressac_red;

  wave_count += 1;
  if (wave_count > WAVE_COUNT_CHANGE) {
    wave_count = 0;
    if (FISHMODE == 2) FISHMODE = 1;
    else FISHMODE = 2;
  }
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
  wave_blue   = new Wave(     colorBLUE,  CRGB(0, 0, 0),       NUM_LEDS,       57, EASE_QUAD );     // create wave (color, size, speed, easePOW)
  ressac_green  = new Wave(   colorGREEN, colorBLUE,    NUM_LEDS*0.3,   75, EASE_QUATR );     // create wave (color, size, speed, easePOW)
  ressac_yellow = new Wave(   colorYELW, colorBLUE,    NUM_LEDS*0.3,   75, EASE_QUATR );     // create wave (color, size, speed, easePOW)
  ressac_red    = new Wave(   colorRED, colorBLUE,    NUM_LEDS*0.3,   75, EASE_QUATR );     // create wave (color, size, speed, easePOW)
  wavereset();
  waveclock.attach_ms(25, waveanimator );


  // MODE 1: 4 fish + speed CTRL
  if (FISHMODE == 1)
    for (int i=0; i<4; i++) 
      fishnet[i].run(15, random(100, 250), random(NUM_LEDS), true); // size, speed, position, loop
  

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
  int diff = getEncoderDiff() * ENCODER_SPEED_FACTOR/lapse ;
  if (encoderReverse) diff = -1*diff;
  // if (diff != 0) Serial.println(diff);

  //
  // MODE 1: 4 fish + speed CTRL
  //
  if (FISHMODE == 1) {

    // Move FISHNET with encoder
    if (diff != 0) 
      for (int i=0; i<MAX_FISH; i++) 
        fishnet[i].speedAnim( diff );   

  }

  //
  // MODE 2: 1-3 fish + fishing CTRL
  //
  else if (FISHMODE == 2) {

    // check if enough fish are going up, add if necessary
    if (countRunningFish() < 2 || !wait(8, random(12000, 15000))) 
       findDeadFish()->run(6, random(100, 150), 1, false);

    // accelerate
    if (diff > 0) 
      for (int i=0; i<MAX_FISH; i++)
        fishnet[i].speedAnim( diff ); 
    
    // decelerate
    if (diff < 0) {
      for (int i=0; i<MAX_FISH; i++) fishnet[i].decel();
    }

    // add new fish
    if (diff > 800)
      if (countRunningFish() < MAX_FISH) 
        if (wait(9, 200));
        else findDeadFish()->run(6, random(150, 400), 1, false);
        
  }

  delay(15);  // this delay influences ENCODER speed factor
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
  else if (wait(0,100,false));

  // DECAYING 50->100%
  else if (wave_blue->progress() < 100) {
    if (wave_ressac->progress() < 100) wave_ressac->animate();
    if (wave_ressac->progress() > 70) wave_blue->animate();
  }

  // BLACK PAUSE
  else if (wait(1,pauseBLACK));

  // RESET 
  else wavereset();

}


//
// DRAW (Core 0)
//
void draw() {
  FastLED.clear();
  wave_blue->draw(leds);
  wave_ressac->draw(leds);
  for (int i=0; i<MAX_FISH; i++) fishnet[i].draw( leds );
  FastLED.show();
  delay(1);
}



// Fish helpers
int countRunningFish() {
  int c = 0;
  for (int i=0; i<MAX_FISH; i++) 
    if (fishnet[i].running) c += 1;
  return c;
}

Fish* findDeadFish() {
  for (int i=0; i<MAX_FISH; i++) 
    if (!fishnet[i].running) return &fishnet[i];
  return NULL;
}
