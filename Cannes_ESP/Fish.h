#pragma once

#define FISH_ANIMATE_FREQ  25           // Time between to animation Frame
#define FISH_POSITION_ACCURACY 100      // Position increment for 1 pixel

#define FISH_RESET 30000

#define COLLING_TRAIL 130    // 140
#define COLLING_BOID  70     // 70
#define SPARK_CHANCE 45

#include "Ticker.h"

class Fish {

    public:
        bool running = false;
        
        //
        // Constructor (with default values)
        //
        Fish()
        { 
            _color = CRGB(255,255,255);
            _direction = 1;
            _position = 0;
            _speedMin = 50;
            _speed = 25;
            _size = 15;

            _coolingBoid = COLLING_BOID;
            _coolingTrailer = COLLING_TRAIL;
            _sparkChance = SPARK_CHANCE;  //45
            _sparkBrightness = 250;
            

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
            if (steps > 0) _direction = 1;
            else if (steps < 0) _direction = -1;
            else return;

            _position += steps;
            
            if (_doLoop) {
                if (_position < 0) _position += NUM_LEDS*FISH_POSITION_ACCURACY;
                else _position %= NUM_LEDS*FISH_POSITION_ACCURACY;
            }
            else {
                if (_position < 0 || _position >= NUM_LEDS*FISH_POSITION_ACCURACY) {
                    running = false;
                }
            } 
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
        // Move 1 step in set _direction
        //
        void move() {
            move(_direction);
        }

        //
        //  Jump to _position
        //
        void jump(int pos) {
            running = true;
            // _speedMin= random(50,100);
            _direction = 1;
            _position = (pos%NUM_LEDS)*FISH_POSITION_ACCURACY;
        }

        void size(int s) {
            _size = s;
        }

        void looping(bool dd) {
            _doLoop = dd;
        }

        void speed(int s) {
            _speedMin = s;
            _speed = s;
        }

        void breaks() {
            _speed = 0;
        }

        void decel() {
            _speed /= 1.5;
        }

        void accelerate() {
            _speedMin *= 1.05;
        }

        void stop() {
            running = false;
        }

        void run(int size_, int speed_, int position_, int doloop_) {
            size(size_);
            speed(speed_);
            looping(doloop_);
            jump(position_);
        }

        //
        // Color change
        //
        void setColor(CRGB colo) {
            _color = colo;
        }

        //
        // Position in PIXEL
        //
        int getPositionPixel() {
            return _position/FISH_POSITION_ACCURACY;
        }

        //
        // Speed Goal
        //
        void speedAnim(int s) {
            speedAnim(s, true);
        }

        void speedAnim(int s, bool externalControl) {
            
            int accelSmoothing = 6;
            int decelSmoothing = 7;

            // smoothing _speed change
            if (abs(s) > abs(_speed)) _speed = (s+_speed*accelSmoothing)/(accelSmoothing+1); // Accelerate
            else _speed = (s+_speed*decelSmoothing)/(decelSmoothing+1); // Decelerate

            if (externalControl) lastMove = millis();

            Serial.println(_speed);
        }

    


    private:

        //
        //  Internal stuffs
        //
        CRGB _color;         // _color of the fish
        int _size;           // length of the base fish
        long _position;      // _position (x1000 pixel)   _position=2000  =>  pixel=2  see FISH_POSITION_ACCURACY
        int _direction;      // _direction (1 / -1)  
        int _speed;          // as _position change per frame
        int _speedMin;
        bool _doLoop = false; 
        

        int _coolingBoid;        // Crawler dimming _speed 
        int _coolingTrailer;     // Trail dimming _speed
        int _sparkChance;        // Chance to spark: generate dust when moving 
        int _sparkBrightness;    // Sparks brightness (% of crawler main brightness)   

        Ticker clock;

        unsigned long lastMove = 0; // last time the fish was controlled

        byte boid[NUM_LEDS];
        CRGB fulltrail[NUM_LEDS];

        // Clocked callback
        static void animator(Fish *pThis) { pThis->animate(); }

        //
        // Update internal animation
        //
        void animate() {

            // Direction Wacthdog
            if ((millis()-lastMove) > FISH_RESET && _direction != 1) {
                _speed=0;
                speedAnim(1*_speedMin);
            }

            // Move SPEED
            //if (_speed != 0) {
                move(_speed/2);
                speedAnim(_direction*_speedMin, false);
            //}

            // FULLTRAIL Fade
            for(int j=0; j<NUM_LEDS; j++) 
                if( (random8(10)>5) ) 
                    fulltrail[j].fadeToBlackBy( _coolingTrailer ); 

            // Running
            if (!running) return;

            // BOID Cool down every cell a little
            int cooldown;
            for( int i = 1; i < _size; i++) {
                cooldown = random16( ((_coolingBoid * 10) / _size) + 2);
                if(cooldown>boid[i]) boid[i]=0;
                else boid[i]=boid[i]-cooldown;
            }

            // BOID Heat from each cell drifts 'up' and diffuses a little
            for( int k= _size - 1; k >= 2; k--) 
                boid[k] = (boid[k - 1] + 2*boid[k - 2]) / 3;
            boid[1] = random8(200, 240);
            boid[0] = random8(220, 255);

            // BOID TAIL Randomly ignite new 'sparks'
            if( random8() < _sparkChance ) boid[random16(_size/2, _size)] = _sparkBrightness;

            // DRAW boid into fulltrail
            int mIndex;
            for(int j = 0; j < _size; j++) {
                mIndex = (getPositionPixel()-(j*_direction))%NUM_LEDS;
                if (mIndex < 0) mIndex += NUM_LEDS;
                fulltrail[mIndex] = CRGB( scale8(_color.r, boid[j]), scale8(_color.g, boid[j]), scale8(_color.b, boid[j]));
            }

        }
    
};
