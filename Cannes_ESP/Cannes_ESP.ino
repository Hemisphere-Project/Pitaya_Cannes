
// PINS SUR LA CARTE: 23 22 11 5 21 19

// LED STRIP
#include "FastLED.h"
#define LED_PIN     23
#define NUM_LEDS    1600
#define BRIGHTNESS  200
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

// ROTARY ENCODER
int encoderPin1 = 21;
int encoderPin2 = 19;
volatile int lastEncoded = 0;
volatile long encoderValue = 0;
volatile long encoderAngle = 0;


void setup() {

  delay(2000);
  // LED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );

  // CODER
  pinMode(encoderPin1, INPUT_PULLUP);
  pinMode(encoderPin2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderPin1), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPin2), updateEncoder, CHANGE);

  // Serial.begin(115200);

}

int xsnake1;
int xstep = 5;
int snakeLength = 50;

float phi = 0;
float phistep = 0.1;

unsigned long timeNow;

void loop() {

  FastLED.clear ();

  // timeNow = micros();

  int visu = map(encoderValue, 0, 799, 0, NUM_LEDS);
  int intensity = map(encoderValue, 0, 799, 0, 255);
    Serial.println(visu);
  for(int i=0; i<=visu; i++) {
    // leds[i] = CRGB(intensity,intensity,intensity);
    leds[i] = CRGB(255,255,255);
  }

  // oscillate
  // for(int i=0; i<=NUM_LEDS; i++) {
  //   uint8_t col = 150*(sin(phi)+1)/2;
  //   // int c = int(col)
  //   leds[i] = CRGB(0,0,150);
  // }
  // phi = phi+phistep;
  // if(phi>2*PI){ phi=0;}

  // HOW TO MULTIPLY / ADD one effect over the other ??

  // snake 1
  for(int i=0; i<=snakeLength; i++) {
    leds[xsnake1+i] = CRGB(255*i/snakeLength,255*i/snakeLength,255*i/snakeLength);
  }
  xsnake1 = xsnake1+xstep;
  if(xsnake1>1500){ xsnake1=0;}



  // showtime
  FastLED.show();
  delay(1);
}
