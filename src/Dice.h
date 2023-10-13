//
// Created by marcel on 13.10.23.
//

#ifndef XCLOCK_DICE_H
#define XCLOCK_DICE_H

#include <Arduino.h>

extern "C" {
class Dice {
    static long laufzeit;

    static void task(void *pvParameters);

    static TaskHandle_t taskHandle;
public:

    static bool running;
    static bool stop;

    static void run(long duration);

    static void process_button_click(uint8_t button, uint8_t type);
};

}
#endif //XCLOCK_DICE_H
