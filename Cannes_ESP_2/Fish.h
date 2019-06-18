
#define CRAWLER_COOLING 90      // Crawler dimming speed 

#define SPARKS_CHANCE 40        // Chance to spark: generate dust when moving 
#define SPARKS_BRIGHTNESS 90    // Sparks brightness (% of crawler main brightness)   

#define TRAILER_COOLING 70      // Trail dimming speed


class Fish {

    public:
        //
        // Constructor
        //
        Fish(int size) : sizeBoid(size) {
            colorBoid = CRGB(255,255,255);
        }        
        
        //
        // Draw
        //
        void draw(CRGB* leds) {

            // Animate
            if ((millis()-lastUpdate) > updateFreq) {
                animate();
                lastUpdate = millis();
            }

            // draw boid into meteor
            int mIndex;
            for(int j = 0; j < sizeBoid; j++) {
                mIndex = (posBoid-(j*dirBoid))%NUM_LEDS;
                if (mIndex < 0) mIndex += NUM_LEDS;
                meteor[mIndex] = CRGB( scale8(colorBoid.r, boid[j]), scale8(colorBoid.g, boid[j]), scale8(colorBoid.b, boid[j]));
            }
        
            // draw on LEDS
            for( int j = 0; j < NUM_LEDS; j++) 
                leds[j] += meteor[j];
        }

        //
        // Move Forward
        //
        void move_up() {
            dirBoid = 1;
            move();
        }

        //
        // Move Backward
        //
        void move_down() {
            dirBoid = -1;
            move();
        }

        //
        // Move in last direction
        //
        void move() {
            posBoid += dirBoid;
            if (posBoid < 0) posBoid += NUM_LEDS;
            else posBoid %= NUM_LEDS;
        }

        //
        //  Jump to position
        //
        void jump(int pos) {
            posBoid = pos%NUM_LEDS;
        }

        //
        // Color change
        //
        void color(CRGB colo) {
            colorBoid = colo;
        }



    private:

        //
        //  Internal stuffs
        //
        int sizeBoid;           // length of the boid (the base fish) - set in constructor
        int posBoid = 0;       // position of boid 
        int dirBoid = 1;        // direction (1 / -1)  

        CRGB colorBoid;         // color of the fish

        uint32_t lastUpdate = 0;   
        int updateFreq = 50;    // time (ms) between animation frame  

        byte boid[NUM_LEDS];
        CRGB meteor[NUM_LEDS];


        //
        // Update internal animation
        //
        void animate() {
    
            // TRAIL Fade
            for(int j=0; j<NUM_LEDS; j++) 
                if( (random8(10)>5) ) 
                    meteor[j].fadeToBlackBy( TRAILER_COOLING ); 

            // BOID Cool down every cell a little
            int cooldown;
            for( int i = 1; i < sizeBoid; i++) {
                cooldown = random16( ((CRAWLER_COOLING * 10) / sizeBoid) + 2);
                if(cooldown>boid[i]) boid[i]=0;
                else boid[i]=boid[i]-cooldown;
            }

            // BOID Heat from each cell drifts 'up' and diffuses a little
            for( int k= sizeBoid - 1; k >= 2; k--) 
                boid[k] = (boid[k - 1] + 2*boid[k - 2]) / 3;
            boid[1] = random8(180, 230);
            boid[0] = random8(220, 255);

            // Randomly ignite new trail 'sparks'
            if( random8() < SPARKS_CHANCE ) boid[random16(sizeBoid/2, sizeBoid)] = 255*SPARKS_BRIGHTNESS/100;

        }
    
};
