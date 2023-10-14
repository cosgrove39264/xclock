//
// Created by marcel on 26.09.23.
//

#include "Clock.h"
#include "WLan.h"
#include "Display.h"
#include "RunningPixel.h"
#include "SetupMode.h"
#include "ArduinoNvs.h"

TaskHandle_t Clock::clockTaskHandle = nullptr;
uint8_t Clock::mode = 0;
bool Clock::needs_repaint = false;
bool Clock::dst = false;
bool Clock::ntpinited = false;

void Clock::init() {
    Serial.println("Initialisiere Clock");

    Clock::mode = NVS.getInt("cmode");
    Clock::dst = NVS.getInt("dst") > 1;

    Clock::ntpinited = false;

    xTaskCreate(Clock::task,
                "Clock",
                4096,
                nullptr,
                10,
                &Clock::clockTaskHandle);

}

void Clock::task(void *pvParameters) {
    time_t now;
    struct tm timeinfo{};
    char buffer[80];
    Serial.println("Starte Clock Task");
    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", Clock::dst ? 7200 : 3600, 60000);
    ntpinited = false;
    int old_seconds = 0;
    int old_minutes = 1;
    int minutes = 0;
    int hours = 0;
    int seconds = 0;

    unsigned long last_update = 0;

    for (;;) {

        if(SetupMode::ota_started){
            break;
        }

        if (WLan::connected && (!ntpinited || (millis() - last_update > 60000))) {
            if (!ntpinited) {
                Serial.println("WLan Connected Syncronisiere mit NTP");
                Serial.println("DST Status: " + String(Clock::dst));
                timeClient.begin();
                ntpinited = true;
                RunningPixel::stop = true;
            }

            if (!timeClient.update()) {
                timeClient.forceUpdate();
            } else if (timeClient.isTimeSet()) {
                last_update = millis();
                Serial.println("Update RTC from NTP");
                Serial.println(timeClient.getFormattedTime());
                unsigned long epochTime = timeClient.getEpochTime();
                struct tm *ptm = gmtime((time_t *) &epochTime);
                strftime(buffer, 80, "%A, %B %d %Y %H:%M:%S", ptm);
                Serial.println(String(buffer));
                struct timeval tv_now{};
                tv_now.tv_sec = epochTime;
                tv_now.tv_usec = 0;
                settimeofday(&tv_now, NULL);
            }
        } else if (!WLan::connected) {
            timeClient.end();
            ntpinited = false;
        }
        if (SetupMode::active || Display::effect_running || Display::pause) {
            vTaskDelay(500 / portTICK_PERIOD_MS);
            needs_repaint = true;
            continue;
        }

        time(&now);
        localtime_r(&now, &timeinfo);

        hours = random(0, 23);
        minutes = random(0, 59);
        seconds = random(0, 59);

        hours = timeinfo.tm_hour;
        minutes = timeinfo.tm_min;
        seconds = timeinfo.tm_sec;

        if (old_seconds == seconds) {
            vTaskDelay(150 / portTICK_PERIOD_MS);
            continue;
        }
        old_seconds = seconds;
        int n = hours / 10;
        int o = hours % 10;
        int m = minutes / 10;
        int p = minutes % 10;
       /* strftime(buffer, 80, "%A, %B %d %Y %H:%M:%S", &timeinfo);
        Serial.print("Now: " + String(buffer) + " *** ");
        Serial.print(String(n) + "  ");
        Serial.print(String(o) + "  ");
        Serial.print(String(m) + "  ");
        Serial.println(String(p) + "  ");
*/

        if (hours == 0 && minutes == 0) {
            RunningPixel::run(7);
        } else {
            RunningPixel::stop = true;
            if (minutes != old_minutes || needs_repaint) {
                if (WLan::connected) {
                    switch (Clock::mode) {
                        case 1:
                            Display::fill_segment(0, (uint8_t) n, 150, 0, 0);
                            break;
                        case 2:
                            Display::fill_segment_dice(0, (uint8_t) n, 150, 0, 0);
                            break;
                        default:
                            Display::fill_segment_random(0, (uint8_t) n, 150, 0, 0);
                    }
                } else {
                    switch (Clock::mode) {

                        case 1:
                            Display::fill_segment(0, (uint8_t) n, 255, 255, 255);
                            break;
                        case 2:
                            Display::fill_segment_dice(0, (uint8_t) n, 255, 255, 255);
                            break;
                        default:
                            Display::fill_segment_random(0, (uint8_t) n, 255, 255, 255);
                    }
                }
                switch (Clock::mode) {
                    case 1:
                        Display::fill_segment(1, (uint8_t) o, 150, 150, 0);
                        break;
                    case 2:
                        Display::fill_segment_dice(1, (uint8_t) o, 150, 150, 0);
                        break;
                    default:
                        Display::fill_segment(1, (uint8_t) o, 150, 150, 0);
                }

                if (minutes == 55) {
                    Display::fill_segment(2, (uint8_t) m, 0, 0, 150);
                    Display::fill_segment_dice(3, (uint8_t) p, 150, 0, 150);

                } else {
                    switch (Clock::mode) {
                        case 1: {
                            Display::fill_segment(2, (uint8_t) m, 0, 0, 150);
                            Display::fill_segment(3, (uint8_t) p, 150, 0, 150);
                        }
                            break;
                        case 2: {
                            Display::fill_segment_dice(2, (uint8_t) m, 0, 0, 150);
                            Display::fill_segment_dice(3, (uint8_t) p, 150, 0, 150);
                        }
                            break;
                        default: {
                            Display::fill_segment_random(2, (uint8_t) m, 0, 0, 150);
                            Display::fill_segment_random(3, (uint8_t) p, 150, 0, 150);
                        }
                    }
                }
                Display::update();
                old_minutes = minutes;
            }
        }
        needs_repaint = false;
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void Clock::next_mode() {
    Clock::mode++;
    if (Clock::mode > 2) {
        Clock::mode = 0;
    }

    Serial.println("Clock-Mode now: " + String(Clock::mode));


    NVS.setInt("cmode", Clock::mode, true);

    Clock::needs_repaint = true;
}

void Clock::process_button_click(uint8_t button, uint8_t type) {

    if (SetupMode::active) {
        return;
    }

    if (button == 0) {
        if (type == 1) {
            Clock::next_mode();
        }
    } else if (button == 3) {
        if (type == 4 || type == 5) {
            Clock::dst = !Clock::dst;
            NVS.setInt("dst", Clock::dst ? 2 : 1, true);
            Clock::ntpinited = false;
            SetupMode::reboot();
        }
    }
}


