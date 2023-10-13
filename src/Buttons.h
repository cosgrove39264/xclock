//
// Created by marcel on 11.10.23.
//

#ifndef XCLOCK_BUTTONS_H
#define XCLOCK_BUTTONS_H

#include <Arduino.h>

extern "C" {
class Buttons {
protected:
    static TaskHandle_t taskHandle;

    static void task(void *pvParameters);

    static uint8_t buttonState[4];
    static int currentState[4];


public:
    static void init();


    static bool is_pressed(uint8_t button);
};

}
#endif //XCLOCK_BUTTONS_H
