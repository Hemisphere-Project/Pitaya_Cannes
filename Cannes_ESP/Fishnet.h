#pragma once

#include "Fish.h"

#define MAX_FISH 20

class Fishnet {
    public:
        Fish* add(Fish* newfish) {
            if (index == MAX_FISH-1) return NULL;
            index += 1;
            net[index] = newfish;
            return net[index];
        }

        Fish* get(byte i) {
            if (i >= MAX_FISH) return NULL;
            return net[i];
        }

        void draw(CRGB* l) {
            for(int i=0; i<MAX_FISH; i++)
                if (net[i]) net[i]->draw(l);
        }

        void speedAnim(int s) {
            for(int i=0; i<MAX_FISH; i++)
                if (net[i]) net[i]->speedAnim(s);
        }

    private:
        Fish* net[MAX_FISH];
        int index = 0;

};
