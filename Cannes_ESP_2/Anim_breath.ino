float phi = 0;
float phistep = 0.05;

void breath(CRGB *leds) {
    // oscillate
    for(int i=0; i<=NUM_LEDS; i++) {
    uint8_t col = 150*(sin(phi)+1)/2;
    // int c = int(col)
    leds[i] += CRGB(0,0,col);
    }
    phi = phi+phistep;
    if(phi>2*PI){ phi=0;}
}
