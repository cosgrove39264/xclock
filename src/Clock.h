//
// Created by marcel on 26.09.23.
//

#ifndef XCLOCK_CLOCK_H
#define XCLOCK_CLOCK_H

#include <Arduino.h>

extern "C" {
class Clock {
protected:
    static uint8_t mode;
    static bool needs_repaint;
    static bool dst;
public:
    static bool ntpinited;

    static void init();

    static TaskHandle_t clockTaskHandle;

    static void task(void *pvParameters);

    static void next_mode();


    static void process_button_click(uint8_t button, uint8_t type);
};

}
#endif //XCLOCK_CLOCK_H
