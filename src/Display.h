//
// Created by marcel on 26.09.23.
//

#ifndef XCLOCK_DISPLAY_H
#define XCLOCK_DISPLAY_H

#include <FastLED.h>

class Display {
protected:

    static CRGB pixels[27];
    static uint8_t *backup;
public:
    static const int NUMPIXELS = 27;
    static const int PIN = 26;
    static bool effect_running;
    static bool pause;
    static uint8_t brightness;


    static void init();

    static void clear();

    static void clear_display();

    static void update();

    static void set_color(int x, int y, uint8_t r, uint8_t g, uint8_t b);

    static void set_color(int pos, uint8_t r, uint8_t g, uint8_t b);

    static void fill_segment(uint8_t segment, uint8_t num_pixel, uint8_t r, uint8_t g, uint8_t b);


    static void fill_segment_dice(uint8_t segment, uint8_t num_pixel, uint8_t r, uint8_t g, uint8_t b);

    static void clear_pixel(int x, int y);

    static void clear_pixel(int pos);

    static void clear_segment(uint8_t segment);

    static void fill_segment_random(uint8_t segment, uint8_t num_pixel, uint8_t r, uint8_t g, uint8_t b);

    static void fill(uint8_t r, uint8_t g, uint8_t b);

    static bool is_clear(uint8_t x, uint8_t y);

    static bool is_clear(int pos);

    static void start_pause();

    static void resume();


    static void start_effect();

    static void fade_out(int pos, uint8_t scale);

    static void fade_in(int pos, uint8_t scale);

    static void process_button_click(uint8_t button, uint8_t type);

};


#endif //XCLOCK_DISPLAY_H
