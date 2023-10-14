//
// Created by marcel on 11.10.23.
//

#include "Buttons.h"
#include "Display.h"
#include "RunningPixel.h"
#include "Clock.h"
#include "SetupMode.h"
#include "Dice.h"

TaskHandle_t Buttons::taskHandle = nullptr;
uint8_t Buttons::buttonState[] = {0, 0, 0, 0};
int Buttons::currentState[] = {0, 0, 0, 0};

void Buttons::init() {
    Serial.println("Initialisiere Buttons");
    xTaskCreate(Buttons::task,
                "Buttons",
                4096,
                nullptr,
                10,
                &Buttons::taskHandle);

}

void Buttons::task(void *pvParameters) {
    Serial.println("Starte Buttons");
    gpio_num_t buttons[] = {GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_18, GPIO_NUM_19};

    for (int btn = 0; btn < (sizeof(buttons) / sizeof(gpio_num_t)); btn++) {
        pinMode(buttons[btn], INPUT_PULLUP);
    }


    unsigned long short_press_time = 250;
    unsigned long long_press_time = 1500;
    unsigned long longer_press_time = 5000;
    unsigned long reset_press_time = 15000;


    int lastState[] = {0, 0, 0, 0};
    unsigned long pressedTime[] = {0, 0, 0, 0};
    unsigned long releasedTime[] = {0, 0, 0, 0};
    unsigned long lastClickTime[] = {0, 0, 0, 0};

    uint8_t oldButtonState[] = {0, 0, 0, 0};

    unsigned long loops = 0;
    auto teiler = 350;

    for (;;) {
        unsigned long now = millis();
        for (int btn = 0; btn < (sizeof(buttonState) / sizeof(uint8_t)); btn++) {
            loops++;
            currentState[btn] = digitalRead(buttons[btn]);
            if (lastState[btn] == HIGH && currentState[btn] == LOW) {  // button is pressed
                pressedTime[btn] = now;
                buttonState[btn] = 0;
            } else if (lastState[btn] == LOW && currentState[btn] == LOW) {
                long pressDuration = now - pressedTime[btn];
                oldButtonState[btn] = buttonState[btn];
                if (pressDuration < short_press_time) {
                    buttonState[btn] = 1;
                } else if (pressDuration < long_press_time) {
                    buttonState[btn] = 2;
                } else if (pressDuration < longer_press_time) {
                    buttonState[btn] = 3;
                } else if (pressDuration > reset_press_time) {
                    buttonState[btn] = 5;
                } else {
                    buttonState[btn] = 4;
                }

                if (buttonState[btn] > 2) {
                    Display::start_pause();

                    uint x = loops % teiler;
                    if (x < 6) {
                        Display::clear_segment(2);
                        Display::update();
                        vTaskDelay(5 / portTICK_PERIOD_MS);
                    } else if ((x > (teiler / 2) - 3) && x < ((teiler / 2) + 3)) {
                        if (buttonState[btn] == 4) {
                            Display::fill_segment(2, buttonState[btn], 255, 255, 0);
                        } else if (buttonState[btn] == 5) {
                            Display::fill_segment(2, buttonState[btn], 255, 0, 0);
                        } else {
                            Display::fill_segment(2, buttonState[btn], 0, 255, 0);
                        }
                        Display::update();
                        vTaskDelay(5 / portTICK_PERIOD_MS);
                    }
                }

                if (oldButtonState[btn] != buttonState[btn]) {

                }

            } else if (lastState[btn] == LOW && currentState[btn] == HIGH) { // button is released
                releasedTime[btn] = now;
                lastClickTime[btn] = now;
                Display::resume();
                switch (buttonState[btn]) {
                    case 0:
                        break;
                    default:
                        SetupMode::process_button_click(btn, buttonState[btn]);
                        Clock::process_button_click(btn, buttonState[btn]);
                        RunningPixel::process_button_click(btn, buttonState[btn]);
                        Dice::process_button_click(btn, buttonState[btn]);
                        Display::process_button_click(btn, buttonState[btn]);
                }


                buttonState[btn] = 0;
            }

            lastState[btn] = currentState[btn];

        }

        vTaskDelay(2 / portTICK_PERIOD_MS);
    }


    vTaskDelete(NULL);
}

bool Buttons::is_pressed(uint8_t button) {
    if (button > 3) {

        return false;
    }
    return currentState[button] == LOW;
}
