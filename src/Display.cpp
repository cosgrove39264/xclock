//
// Created by marcel on 26.09.23.
//

#include "Display.h"

Adafruit_NeoPixel Display::pixels = Adafruit_NeoPixel(Display::NUMPIXELS,Display::PIN, NEO_GRB + NEO_KHZ800);

void Display::init() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif
    Display::pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

}

void Display::clear() {
    Display::pixels.clear(); // Set all pixel colors to 'off'
    Display::pixels.show(); // Update strip
}
