//
// Created by marcel on 26.09.23.
//

#ifndef TIXCLOCK_DISPLAY_H
#define TIXCLOCK_DISPLAY_H

#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif



class Display {
public:
    static const int NUMPIXELS = 27;
    static const int PIN = 26;

    static void init();

    static void clear();

protected:

    static Adafruit_NeoPixel pixels;

};


#endif //TIXCLOCK_DISPLAY_H
