
int xsnake1 = 0;
int xstep = 1;
int snakeLength = 10;

void snake(CRGB* leds) {

    // snake 1
    for(int i=0; i<snakeLength; i++)
        if ((xsnake1+i) < NUM_LEDS) 
            leds[xsnake1+i] += CRGB(255*i/snakeLength,255*i/snakeLength,255*i/snakeLength);
            
    xsnake1 = xsnake1+xstep;
    if(xsnake1>NUM_LEDS){ xsnake1=0;}

}