//
// Created by marcel on 26.09.23.
//

#ifndef XCLOCK_WLAN_H
#define XCLOCK_WLAN_H

#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "WiFiManager.h"

extern "C" {
class WLan {
protected:
    static WiFiManager wm;
public:
    static bool connected;

    static TaskHandle_t wlanTaskHandle;

    static void init();

    static void task(void *pvParameters);


    static void resetSettings();

    static void init_ota();
};

}

#endif //XCLOCK_WLAN_H
