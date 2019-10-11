#pragma once

#define FISH_ANIMATE_FREQ  50           // Time between to animation Frame
#define FISH_POSITION_ACCURACY 100      // Position increment for 1 pixel

#include "Ticker.h"

class Fish {

    public:
        //
        // Constructor (with default values)
        //
        Fish(int size, int speedMinimum = 30, int coolingBoid = 90, int coolingTrailer = 70, int spark_chance = 40, int spark_brightness = 220) 
            : size(size), speedMin(speedMinimum), coolingBoid(coolingBoid), coolingTrailer(coolingTrailer), sparkChance(spark_chance), sparkBrightness(spark_brightness)
        { 
            color = CRGB(255,255,255);
            direction = 1;
            position = 0;
            speed = speedMinimum;

            // All Black
            for(int i=0; i<NUM_LEDS; i++) {
                fulltrail[i] = CRGB::Black;
                boid[i] = 0;
            }

            // Start Animator Clock
            clock.attach_ms(FISH_ANIMATE_FREQ, &Fish::animator, this );
        }

        
        //
        // Draw
        //
        void draw(CRGB* ll) {
            for( int j = 0; j < NUM_LEDS; j++) 
                ll[NUM_LEDS-1-j] += fulltrail[j];   // draw in reverse
        }

        //
        // Move   (+ forward, - backward)
        //
        void move(int steps) {
            if (steps > 0) direction = 1;
            else if (steps < 0) direction = -1;
            else return;

            position += steps;

            if (position < 0) position += NUM_LEDS*FISH_POSITION_ACCURACY;
            else position %= NUM_LEDS*FISH_POSITION_ACCURACY;
        }

        //
        // Move Forward
        //
        void move_up(int steps=1) {
            steps = abs(steps);
            move(steps);
        }

        //
        // Move Backward
        //
        void move_down(int steps=1) {
            steps = abs(steps);
            move(steps*-1);
        }

        //
        // Move 1 step in set direction
        //
        void move() {
            move(direction);
        }

        //
        //  Jump to position
        //
        void jump(int pos) {
            position = (pos%NUM_LEDS)*FISH_POSITION_ACCURACY;
        }

        //
        // Color change
        //
        void setColor(CRGB colo) {
            color = colo;
        }

        //
        // Position in PIXEL
        //
        int getPositionPixel() {
            return position/FISH_POSITION_ACCURACY;
        }

        //
        // Speed Goal
        //
        void speedAnim(int s) {

            // smoothing speed change
            if (abs(s) > abs(speed)) speed = (s+speed*2)/3; // Accelerate
            else speed = (s+speed*3)/4; // Decelerate
        }


    private:

        //
        //  Internal stuffs
        //
        CRGB color;         // color of the fish
        int size;           // length of the base fish
        long position;      // position (x1000 pixel)   position=2000  =>  pixel=2  see FISH_POSITION_ACCURACY
        int direction;      // direction (1 / -1)  
        int speed;          // as position change per frame
        int speedMin;

        int coolingBoid;        // Crawler dimming speed 
        int coolingTrailer;     // Trail dimming speed
        int sparkChance;        // Chance to spark: generate dust when moving 
        int sparkBrightness;    // Sparks brightness (% of crawler main brightness)   

        Ticker clock;

        byte boid[NUM_LEDS];
        CRGB fulltrail[NUM_LEDS];

        // Clocked callback
        static void animator(Fish *pThis) { pThis->animate(); }

        //
        // Update internal animation
        //
        void animate() {

            // Move SPEED
            if (speed != 0) {
                move(speed);
                speedAnim(direction*speedMin);
            }

            // FULLTRAIL Fade
            for(int j=0; j<NUM_LEDS; j++) 
                if( (random8(10)>5) ) 
                    fulltrail[j].fadeToBlackBy( coolingTrailer ); 

            // BOID Cool down every cell a little
            int cooldown;
            for( int i = 1; i < size; i++) {
                cooldown = random16( ((coolingBoid * 10) / size) + 2);
                if(cooldown>boid[i]) boid[i]=0;
                else boid[i]=boid[i]-cooldown;
            }

            // BOID Heat from each cell drifts 'up' and diffuses a little
            for( int k= size - 1; k >= 2; k--) 
                boid[k] = (boid[k - 1] + 2*boid[k - 2]) / 3;
            boid[1] = random8(200, 240);
            boid[0] = random8(220, 255);

            // BOID TAIL Randomly ignite new 'sparks'
            if( random8() < sparkChance ) boid[random16(size/2, size)] = sparkBrightness;

            // DRAW boid into fulltrail
            int mIndex;
            for(int j = 0; j < size; j++) {
                mIndex = (getPositionPixel()-(j*direction))%NUM_LEDS;
                if (mIndex < 0) mIndex += NUM_LEDS;
                fulltrail[mIndex] = CRGB( scale8(color.r, boid[j]), scale8(color.g, boid[j]), scale8(color.b, boid[j]));
            }

        }
    
};
