//
// Created by marcel on 12.10.23.
//

#ifndef XCLOCK_SETUPMODE_H
#define XCLOCK_SETUPMODE_H

#include <Arduino.h>
#include <HttpsOTAUpdate.h>

extern "C" {
class SetupMode {
protected:
    static int current_step;
    static unsigned long last_action;
    static uint8_t values[12];

    static bool ota_started;

    static void inc_val(uint8_t val, uint8_t mx);

    static void dec_val(uint8_t val, uint8_t mx);

static void HttpEvent(HttpEvent_t *event);


public:
    static bool active;

    static void process_button_click(uint8_t button, uint8_t type);

    static void reboot();

    static void reset();

    static void enable();

    static void check();

    static void next_step();

    static void update_display();

    static void http_ota_task(void *pvParameters);

    static void start_http_ota();
};

}
#endif //XCLOCK_SETUPMODE_H
