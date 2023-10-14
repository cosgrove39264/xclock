//
// Created by marcel on 12.10.23.
//

#include "SetupMode.h"
#include "Display.h"
#include "WLan.h"
#include "Buttons.h"
#include "RunningPixel.h"
#include "Clock.h"
#include "ArduinoNvs.h"
#include <HttpsOTAUpdate.h>
#include "root_ca.h"

int SetupMode::current_step = 0;
unsigned long SetupMode::last_action = 0;
bool SetupMode::active = false;
uint8_t SetupMode::values[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool SetupMode::ota_started = false;
int SetupMode::clength = 0;
int SetupMode::loaded = 0;

void SetupMode::process_button_click(uint8_t button, uint8_t type) {

    if (button == 0) {

        if (type == 4) {
            SetupMode::reboot();
        }
        if (type == 5) {
            SetupMode::reset();
        }

    }
    if (!SetupMode::active) {


        if (button == 1) {
            if (type == 1 || type == 2) {
                if (Buttons::is_pressed(2)) {
                    SetupMode::enable();
                }
            }

        }
        if (button == 2) {
            if (type == 1 || type == 2) {
                if (Buttons::is_pressed(1)) {
                    SetupMode::enable();
                }
            } else if (type == 4 || type == 5) {
                SetupMode::start_http_ota();
            }

        }
    } else {
        SetupMode::last_action = millis();
        if (type == 1 || type == 2) {
            if (button == 0) {
                SetupMode::next_step();
            }
            if (button == 1) {
                switch (SetupMode::current_step) {
                    case 1:
                        inc_val(0, 2);
                        break;
                    case 2:
                        inc_val(1, 9);
                        break;
                    case 3:
                        inc_val(2, 9);
                        break;
                    case 4:
                        inc_val(3, 9);
                        break;
                    case 5:
                        inc_val(4, 1);
                        break;
                    case 6:
                        inc_val(5, 9);
                        break;
                    case 7:
                        inc_val(6, 3);
                        break;
                    case 8:
                        inc_val(7, 9);
                        break;

                    case 9:
                        inc_val(8, 2);
                        break;
                    case 10:
                        inc_val(9, 9);
                        break;
                    case 11:
                        inc_val(10, 5);
                        break;
                    case 12:
                        inc_val(11, 9);
                        break;
                }
            }
            if (button == 2) {
                switch (SetupMode::current_step) {
                    case 1:
                        dec_val(0, 2);
                        break;
                    case 2:
                        dec_val(1, 9);
                        break;
                    case 3:
                        dec_val(2, 9);
                        break;
                    case 4:
                        dec_val(3, 9);
                        break;
                    case 5:
                        dec_val(4, 1);
                        break;
                    case 6:
                        dec_val(5, 9);
                        break;
                    case 7:
                        dec_val(6, 3);
                        break;
                    case 8:
                        dec_val(7, 9);
                        break;

                    case 9:
                        dec_val(8, 2);
                        break;
                    case 10:
                        dec_val(9, 9);
                        break;
                    case 11:
                        dec_val(10, 5);
                        break;
                    case 12:
                        dec_val(11, 9);
                        break;
                }
            }
        }
        SetupMode::update_display();
    }

}

void SetupMode::reboot() {
    Serial.println("Reboot");
    NVS.commit();
    Display::start_pause();
    Display::fill(255, 255, 0);
    Display::update();
    vTaskDelay(50 / portTICK_PERIOD_MS);
    Serial.flush();
    esp_restart();
}

void SetupMode::reset() {
    Serial.println("WiFI Reset");
    Display::start_pause();
    Display::fill(255, 0, 0);
    Display::update();
    vTaskDelay(50 / portTICK_PERIOD_MS);
    NVS.eraseAll(true);
    WLan::resetSettings();
    SetupMode::reboot();
}

void SetupMode::enable() {
    if (SetupMode::active) {
        return;
    }
    Serial.println("Setup-Modus wird aktiviert");
    SetupMode::active = true;
    SetupMode::last_action = millis();
    Display::start_pause();
    Display::clear();
    Display::fill_segment(0, 3, 0, 255, 255);
    Display::update();

    if (Clock::ntpinited) {

        for (int i = 0; i < 5; i++) {

            Display::clear_display();
            Display::fill(255, 0, 0);
            Display::update();
            vTaskDelay(250 / portTICK_PERIOD_MS);
            Display::clear();
            vTaskDelay(250 / portTICK_PERIOD_MS);
        }
        SetupMode::active = false;
        Display::pause = false;
        return;
    }

    time_t now;
    struct tm timeinfo{};

    time(&now);
    localtime_r(&now, &timeinfo);

    SetupMode::values[0] = (timeinfo.tm_year + 1900) / 1000;
    SetupMode::values[1] = ((timeinfo.tm_year + 1900) % 1000) / 100;
    SetupMode::values[2] = (((timeinfo.tm_year + 1900) % 1000) % 100) / 10;
    SetupMode::values[3] = (((timeinfo.tm_year + 1900) % 1000) % 100) % 10;

    SetupMode::values[4] = (timeinfo.tm_mon + 1) / 10;
    SetupMode::values[5] = (timeinfo.tm_mon + 1) % 10;


    SetupMode::values[6] = (timeinfo.tm_mday) / 10;
    SetupMode::values[7] = (timeinfo.tm_mday) % 10;

    SetupMode::values[8] = timeinfo.tm_hour / 10;
    SetupMode::values[9] = timeinfo.tm_hour % 10;

    SetupMode::values[10] = timeinfo.tm_min / 10;
    SetupMode::values[11] = timeinfo.tm_min % 10;


    SetupMode::last_action = millis();
    SetupMode::current_step = 0;
    SetupMode::next_step();
}

void SetupMode::check() {
    if (!SetupMode::active) {
        return;
    }
    if ((millis() - SetupMode::last_action) > 15000) {
        SetupMode::active = false;
        Display::pause = false;
        RunningPixel::run(5);
        return;
    }

}

void SetupMode::next_step() {
    SetupMode::current_step++;

    if (SetupMode::current_step > 12) {
        if (!Clock::ntpinited) {
            struct tm tm{};
            tm.tm_year = (values[0] * 1000 + values[1] * 100 + values[2] * 10 + values[3]) - 1900;

            tm.tm_mon = values[4] * 10 + values[5];

            tm.tm_mday = values[6] * 10 + values[7];

            tm.tm_hour = values[8] * 10 + values[9];

            tm.tm_min = values[10] * 10 + values[11];

            tm.tm_sec = 0;

            time_t t = mktime(&tm);

            printf("Setting time: %s", asctime(&tm));

            struct timeval now = {.tv_sec = t};

            settimeofday(&now, NULL);
        }

        SetupMode::active = false;
        Display::pause = false;
        RunningPixel::run(5);
        return;
    }

    Display::fill(255, 255, 150);
    Display::update();
    vTaskDelay(10 / portTICK_PERIOD_MS);
    SetupMode::update_display();
}

void SetupMode::update_display() {
    Serial.print(String(SetupMode::current_step) + ": ");
    for (int i = 0; i < 11; i++) {
        Serial.print(String(values[i]) + "#");
    }
    Serial.println(String(values[11]));

    Display::clear();
    Display::fill_segment(0, 3, 0, 255, 255);

    switch (SetupMode::current_step) {
        case 1:
        case 2:
        case 3:
        case 4:
            Display::fill_segment(1, 1, 0, 0, 150);
            Display::fill_segment(2, current_step, 0, 150, 0);
            Display::fill_segment(3, SetupMode::values[current_step - 1], 255, 255, 0);
            break;
        case 5:
        case 6:
            Display::fill_segment(1, 2, 0, 0, 150);
            Display::fill_segment(2, current_step - 4, 0, 150, 0);
            Display::fill_segment(3, SetupMode::values[current_step - 1], 255, 255, 0);
            break;
        case 7:
        case 8:
            Display::fill_segment(1, 3, 0, 0, 150);
            Display::fill_segment(2, current_step - 6, 0, 150, 0);
            Display::fill_segment(3, SetupMode::values[current_step - 1], 255, 255, 0);
            break;
        case 9:
        case 10:
            Display::fill_segment(1, 4, 0, 0, 150);
            Display::fill_segment(2, current_step - 8, 0, 150, 0);
            Display::fill_segment(3, SetupMode::values[current_step - 1], 255, 255, 0);
            break;
        case 11:
        case 12:
            Display::fill_segment(1, 5, 0, 0, 150);
            Display::fill_segment(2, current_step - 10, 0, 150, 0);
            Display::fill_segment(3, SetupMode::values[current_step - 1], 255, 255, 0);
            break;

    }
    Display::update();
}

void SetupMode::inc_val(uint8_t val, uint8_t mx) {
    Serial.println("inc_val " + String(val) + " " + String(mx));
    if (SetupMode::values[val] == 254) {
        SetupMode::values[val] = 0;
        return;
    }
    SetupMode::values[val] = (SetupMode::values[val] + 1);
    if (SetupMode::values[val] > mx) {
        SetupMode::values[val] = 0;
    }
}

void SetupMode::dec_val(uint8_t val, uint8_t mx) {
    Serial.println("dec_val " + String(val) + " " + String(mx));
    if (SetupMode::values[val] == 0) {
        SetupMode::values[val] = mx;
        return;
    }
    SetupMode::values[val] = (SetupMode::values[val] - 1);
}

void SetupMode::http_ota_task(void *pvParameters) {
    if (!WLan::connected) {

        return;
    }
    SetupMode::ota_started=true;

    WiFiClient client;


    // The line below is optional. It can be used to blink the LED on the board during flashing
    // The LED will be on during download of one buffer of data from the network. The LED will
    // be off during writing that buffer to flash
    // On a good connection the LED should flash regularly. On a bad connection the LED will be
    // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
    // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
    // httpUpdate.setLedPin(LED_BUILTIN, LOW);

    HttpsOTAStatus_t otastatus;
    HttpsOTA.onHttpEvent(HttpEvent);
    HttpsOTA.begin("https://raw.githubusercontent.com/cosgrove39264/xclock/main/firmware/firmware.bin", root_ca);

    for (;;) {
        otastatus = HttpsOTA.status();
        if (otastatus == HTTPS_OTA_SUCCESS) {
            Serial.println(
                    "Firmware written successfully. To reboot device, call API ESP.restart() or PUSH restart button on device");
            vTaskDelay(500 / portTICK_PERIOD_MS);
            Display::fill(0, 255, 0);
            Display::update();
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            SetupMode::reboot();

        } else if (otastatus == HTTPS_OTA_UPDATING) {
            if (SetupMode::clength > 0) {
                int pct = SetupMode::loaded * 100 / SetupMode::clength;
                int px = (Display::NUMPIXELS * pct) / 100;
                Display::clear_display();
                for (int i = 0; i < px; i++) {
                    Display::set_color(i, 255, 255, 255);
                }
                Display::update();
            }else{
                Display::clear();
            }
        } else if (otastatus == HTTPS_OTA_FAIL) {
            Serial.println("Firmware Upgrade Fail");
            Display::fill(255, 0, 0);
            Display::update();
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            SetupMode::reboot();
        }
    }
    vTaskDelete(NULL);
}


void SetupMode::start_http_ota() {

    if (SetupMode::ota_started) {
        return;
    }
    Display::start_pause();

    TaskHandle_t handle;
    xTaskCreate(SetupMode::http_ota_task,
                "Ota",
                4096,
                nullptr,
                10, &handle);
}

void SetupMode::HttpEvent(esp_http_client_event_t *event) {
    switch (event->event_id) {
        case HTTP_EVENT_ERROR:
            Serial.println("Http Event Error");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            Serial.println("Http Event On Connected");
            break;
        case HTTP_EVENT_HEADER_SENT:
            Serial.println("Http Event Header Sent");
            break;
        case HTTP_EVENT_ON_HEADER:
            Serial.printf("Http Event On Header, key=%s, value=%s\n", event->header_key, event->header_value);
            if (strcmp(event->header_key, "Content-Length") == 0) {
                SetupMode::clength = atoi(event->header_value);
            }
            break;
        case HTTP_EVENT_ON_DATA:
            Serial.printf("Http Event On Data len=%d\n", event->data_len);
            SetupMode::loaded += event->data_len;
            if (SetupMode::loaded >= SetupMode::clength) {
                SetupMode::loaded = SetupMode::clength;
            }

            break;
        case HTTP_EVENT_ON_FINISH:

            Serial.println("Http Event On Finish");
            break;
        case HTTP_EVENT_DISCONNECTED:
            Serial.println("Http Event Disconnected");
            break;
            /*   case HTTP_EVENT_REDIRECT:
                   Serial.println("Http Event Redirect");
                   break;*/
    }
}
