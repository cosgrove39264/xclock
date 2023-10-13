#include <Arduino.h>
#include "Display.h"
#include "Clock.h"
#include "WLan.h"
#include "Buttons.h"
#include "SetupMode.h"
#include "ArduinoNvs.h"
#include <esp_ota_ops.h>

bool marked = false;

void setup() {
    NVS.begin();

    Serial.begin(115200);
    while (!Serial);


    Buttons::init();
    Display::init();
    Clock::init();
    WLan::init();
}


void complete_ota() {
    if (millis() < 30000 || marked) {
        return;
    }


    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        Serial.println("Get State Partition was Successfull " + String(ota_state));
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            esp_ota_mark_app_valid_cancel_rollback();
            if (!Display::pause) {
                Display::start_pause();
                Display::fill(0, 255, 0);
                Display::update();
                delay(500);
                Display::resume();
            }
            marked = true;
            Serial.println("marked as complete");
        }
    }
}


void loop() {
    SetupMode::check();
    complete_ota();


    delay(150);
}