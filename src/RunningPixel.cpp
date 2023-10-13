//
// Created by marcel on 09.10.23.
//

#include "RunningPixel.h"
#include "Display.h"
#include "SetupMode.h"

bool RunningPixel::running = false;
bool RunningPixel::stop = false;

long RunningPixel::laufzeit = 10;
TaskHandle_t RunningPixel::taskHandle = nullptr;

void RunningPixel::run(long duration) {
    RunningPixel::laufzeit = duration;
    xTaskCreate(RunningPixel::task,
                "RunningPixel",
                4096,
                nullptr,
                10,
                &RunningPixel::taskHandle);
}

void RunningPixel::task(void *pvParameters) {

    Serial.println("Starte RunningPixel Task");
    RunningPixel::running = true;
    RunningPixel::stop = false;
    Display::effect_running = true;
    Display::clear();
    unsigned long now = millis();
    int pos = -1;
    int dir = 1;

    auto r = (int8_t) ((random(50, 200) + 10) % 255);
    auto g = (int8_t) ((random(50, 200) + 10) % 255);
    auto b = (int8_t) ((random(50, 200) + 10) % 255);

    while (RunningPixel::running) {
        if (SetupMode::active) {
            break;
        }
        if (!Display::pause) {
            pos += dir;
            if (dir > 0) {
                if (pos > Display::NUMPIXELS + 1) {
                    dir = -1;
                }
            } else {
                if (pos < -1) {
                    dir = 1;
                }
            }

            Display::clear_display();
            Display::set_color(pos, r, g, b);
            Display::set_color(pos - dir, r, g, b);
            Display::fade_out(pos - dir, 128);
            Display::set_color(pos - dir - dir, r, g, b);
            Display::fade_out(pos - dir - dir, 64);
            Display::fade_out(pos - dir - dir, 64);

            Display::update();
        }

        unsigned long lz = ((millis() - now) / 1000);

        if (lz >= RunningPixel::laufzeit) {
            RunningPixel::stop = true;
        }

        if (RunningPixel::stop) {
            break;
        }
        vTaskDelay(150 / portTICK_PERIOD_MS);
    }
    RunningPixel::running = false;
    Display::clear();
    Display::effect_running = false;
    Serial.println("RunningPixel Task beendet");
    vTaskDelete(nullptr);
}

void RunningPixel::process_button_click(uint8_t button, uint8_t type) {
    if (SetupMode::active) {
        return;
    }

    if (button == 0) {
        if (type == 2) {

            if (RunningPixel::running) {
                RunningPixel::laufzeit += 7;
                return;
            }
            RunningPixel::run(7);
        }
    }

}

