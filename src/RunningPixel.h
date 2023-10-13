//
// Created by marcel on 09.10.23.
//

#ifndef XCLOCK_RUNNINGPIXEL_H
#define XCLOCK_RUNNINGPIXEL_H

#include <Arduino.h>

extern "C" {
class RunningPixel {
protected:
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
#endif //XCLOCK_RUNNINGPIXEL_H
