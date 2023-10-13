//
// Created by marcel on 13.10.23.
//

#include "Dice.h"
#include "SetupMode.h"
#include "Display.h"
#include "Clock.h"

bool Dice::running = false;
bool Dice::stop = false;

long Dice::laufzeit = 10;
TaskHandle_t Dice::taskHandle = nullptr;

void Dice::run(long duration) {
    Dice::laufzeit = 5;
    xTaskCreate(Dice::task,
                "Dice",
                4096,
                nullptr,
                10,
                &Dice::taskHandle);
}

void Dice::process_button_click(uint8_t button, uint8_t type) {
    if (SetupMode::active) {
        return;
    }
    if (Display::effect_running && !Dice::running) {
        return;
    }


    if (button == 3) {
        if (type == 1 || type == 2) {
            Dice::stop = true;
            while (Dice::running) {
                vTaskDelay(15 / portTICK_PERIOD_MS);
            }
            Dice::run(15);
        }
    }
}

void Dice::task(void *pvParameters) {

    Serial.println("Starte Dice Task");
    Dice::running = true;
    Dice::stop = false;
    Display::start_effect();
    Display::clear();
    unsigned long now = millis();

    uint8_t r1 = (uint8_t) (random(3, 12) % 6) + 1;
    uint8_t r2 = (uint8_t) (random(6, 18) % 6) + 1;

    int rounds = random(50, 260);
    for (int r = 0; r < rounds; r++) {
        r1 = (uint8_t) (random(3, 12) % 6) + 1;

        vTaskDelay(1 / portTICK_PERIOD_MS);
        r2 = (uint8_t) (random(6, 18) % 6) + 1;
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }


    while (Dice::running) {
        if (SetupMode::active) {
            break;
        }
        if (!Display::pause) {
            Display::clear_display();
            Display::fill_segment_dice(1, r1, 0, 0, 255);
            Display::fill_segment_dice(3, r2, 255, 0, 255);
            Display::update();
        }

        unsigned long lz = ((millis() - now) / 1000);

        if (lz >= Dice::laufzeit) {
            Dice::stop = true;
        }

        if (Dice::stop) {
            break;
        }
        vTaskDelay(125 / portTICK_PERIOD_MS);
    }
    Dice::running = false;
    Display::clear();
    Display::effect_running = false;
    Serial.println("Dice Task beendet");
    vTaskDelete(nullptr);
}