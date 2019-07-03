

//
// SETTINGS
//
int phiStepMin = 50;    // speed MIN
int phiStepMax = 100;   // speed MAX

int pauseMin = 2;   // black pause at the end MIN   (% of wave duration)
int pauseMax = 10;  // black pause at the end MAX   (% of wave duration)

int headSize = 15;      // size of the black head           (% of NUM_LEDS)
int mixedSize = 5;      // size of the color mix sea->tail  (% of NUM_LEDS)
int ressacSize = 10;    // size of the full color ressac    (% of NUM_LEDS)


CRGB seaColor(0, 10, 40);       // main color SEA
CRGB ressacColor(10, 139, 10);  // color RESSAC


int32_t phi = 0;
int phiStep = random16(phiStepMin, phiStepMax);
int32_t maxPhi = 32767 * (1 + random8(pauseMin, pauseMax)/10.0);


void wave_progress() {
    phi = phi+phiStep;
    if(phi>maxPhi){ 
        phi=0;
        phiStep = random16(phiStepMin, phiStepMax);
        maxPhi = 32767 * (1 + random8(pauseMin, pauseMax)/10.0);
    }
}


void wave_draw(CRGB *leds) {


    // just breath
    int headPix = NUM_LEDS*headSize/100;
    int headPos = (NUM_LEDS+headPix) * sin16(phi) / 32767;  // 0 -> (NUM_LEDS+headPix)

    // Draw HEAD
    for (int i=0; i<headPix; i++) {
        int k = i+headPos-headPix;
        if (k >= 0 && k < NUM_LEDS) 
            leds[k] = CRGB( 
                map(i, 0, headPix, seaColor.r, 0), 
                map(i, 0, headPix, seaColor.g, 0), 
                map(i, 0, headPix, seaColor.b, 0));

    }

    // Draw BODY
    for (int k=0; k<(headPos-headPix); k++) leds[k] = seaColor;

    // Draw TAIL
    int mixedPix = NUM_LEDS*mixedSize/100;
    int ressacPix = NUM_LEDS*ressacSize/100;
    for (int i=0; i<(ressacPix+mixedPix); i++) {
        int k = i+headPos-(NUM_LEDS+headPix);
        if (k >= 0 && k < NUM_LEDS) 
            if (i < ressacPix) leds[k] = ressacColor;
            else 
              leds[k] = CRGB( 
                map(i, ressacPix, ressacPix+mixedPix, ressacColor.r, seaColor.r), 
                map(i, ressacPix, ressacPix+mixedPix, ressacColor.g, seaColor.g), 
                map(i, ressacPix, ressacPix+mixedPix, ressacColor.b, seaColor.b));
    }

}
