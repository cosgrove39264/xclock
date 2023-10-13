//
// Created by marcel on 26.09.23.
//

#include "WLan.h"
#include "Display.h"
#include "SetupMode.h"
#include <WiFiManager.h>
#include <ArduinoOTA.h>

WiFiManager WLan::wm;

bool WLan::connected = false;
TaskHandle_t WLan::wlanTaskHandle = nullptr;

void WLan::init() {
    Serial.println("Initialisiere WLan");


    xTaskCreate(WLan::task,
                "WLan",
                4096,
                nullptr,
                10,
                &WLan::wlanTaskHandle);
}


void WLan::task(void *pvParameters) {
    Serial.println("Verbinde mit WLAN");

    WiFi.mode(WIFI_MODE_STA);
    bool res = false;
    int tries = 3;
    while (tries > 0) {
        res = WLan::wm.autoConnect("XClock-Wifi-Setup");
        if (res) {
            break;
        }
        tries--;
    }
    if (!res) {
        Serial.println("Failed to connect");
        WLan::resetSettings();
    }

//    WiFi.begin(WLan::ssid.c_str(), WLan::password.c_str());

    for (;;) {
        if (WiFi.status() == WL_CONNECTED) {
            if (!WLan::connected) {
                Serial.println("");
                Serial.println("WiFi connected");
                Serial.println("IP address: ");
                Serial.println(WiFi.localIP());
                Display::start_pause();
                for (int i = 0; i < 3; i++) {
                    Display::clear();
                    vTaskDelay(300 / portTICK_PERIOD_MS);
                    Display::fill_segment(0, 3, 255, 255, 255);
                    Display::fill_segment(1, 9, 255, 255, 255);
                    Display::fill_segment(2, 6, 255, 255, 255);
                    Display::fill_segment(3, 9, 255, 255, 255);
                    Display::update();
                    vTaskDelay(300 / portTICK_PERIOD_MS);
                }
                Display::resume();
                WLan::connected = true;
                WLan::init_ota();
            }

            ArduinoOTA.handle();
        } else if (WLan::connected) {
            WLan::connected = false;
            WiFi.reconnect();
        }

        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void WLan::resetSettings() {
    Serial.println("Wifi Reset Settings");
    wm.resetSettings();
    for (int i = 3; i >= 0; i--) {
        Serial.println("3");
        vTaskDelay(750 / portTICK_PERIOD_MS);
    }
    wm.resetSettings();
    Serial.flush();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    SetupMode::reboot();
}

void WLan::init_ota() {
    ArduinoOTA.onStart([]() {
        Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
}

