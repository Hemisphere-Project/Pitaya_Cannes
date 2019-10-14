#pragma once

#define PHI90  16383
#define PHI180 32767

// EASING
// see https://easings.net/en#  and   https://github.com/ericbarch/arduino-libraries/tree/master/EasingLibrary
#define EASE_SIN    1
#define EASE_QUAD   2
#define EASE_CUBIC  3
#define EASE_QUATR  4


// Faded head size
#define HEADSIZE    60

class Wave {

    public:
        //
        // Constructor (with default values)
        //
        Wave(CRGB color, CRGB colorHead, int size, int speed, int easing)  : color(color), colorHead(colorHead), size(size), speed(speed), easing(easing)
        { 
            reset();
        }

        
        //
        // Draw
        //
        void draw(CRGB* ll) {
            for( int j = 0; j < NUM_LEDS; j++)
                if (fulltrail[j])
                    ll[NUM_LEDS-1-j] = fulltrail[j];   // draw in reverse
        }

        //
        // Update internal animation
        //
        uint32_t animate() {

            if (angle < PHI180) {
                angle += speed;

                // clear buffer
                for( int i = 0; i < NUM_LEDS; i++) fulltrail[i] = CRGB::Black;

                // done
                if (angle >= PHI180) angle = PHI180;

                // head position SIN easing
                //int headPos = (size+HEADSIZE) * sin16(angle) / 32767; 

                // head position easing
                float easeFactor;
                float x;

                if (angle<=PHI90) x = angle * 1.0 / PHI90;
                else x =  (PHI180 - angle) * 1.0 / PHI90;

                _progress = 100 * angle / PHI180;

                if (easing == EASE_SIN) easeFactor = sin(x*M_PI_2);
                else if (easing == EASE_QUAD) easeFactor = -1*x*(x-2);
                else if (easing == EASE_CUBIC)  easeFactor = pow( x-1, 3) + 1;
                else if (easing == EASE_QUATR)  easeFactor = (pow(x-1, 4) - 1)*-1;

                int headPos = (size+HEADSIZE) * easeFactor; 
                
                // Serial.print(angle); Serial.print(" ");
                // Serial.print(x); Serial.print(" ");
                // Serial.println(headPos);  

                // Draw HEAD
                for (int i=0; i<HEADSIZE; i++) {
                    int k = i+headPos-HEADSIZE;
                    if (k >= 0 && k < NUM_LEDS) 
                        fulltrail[k] = CRGB( 
                            map(i, 0, HEADSIZE, color.r, colorHead.r), 
                            map(i, 0, HEADSIZE, color.g, colorHead.g), 
                            map(i, 0, HEADSIZE, color.b, colorHead.b));
                }

                // Draw BODY
                for (int k=0; k<(headPos-HEADSIZE); k++) 
                    if (k < NUM_LEDS) fulltrail[k] = color;
            }
            return angle;
        }

        //
        //  Reset animation
        //
        void reset() {
            angle = 0;
            _progress = 0;
            for(int i=0; i<NUM_LEDS; i++) fulltrail[i] = CRGB::Black;
        }

        // get Progress
        //
        int progress() {
            return _progress;
        }

    private:

        //
        //  Internal stuffs
        //
        CRGB color;         // color of the wave
        CRGB colorHead;     // color of the wave head
        int size;           // max length of the wave
        int speed;          
        int easing;         // easing profile

        int _progress = 0;  // % progress
        uint32_t angle;          // 0->PHI90 = growing   PHI90->PHI180 = descending


        // internal buffer
        CRGB fulltrail[NUM_LEDS];
    
};
