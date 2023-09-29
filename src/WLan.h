//
// Created by marcel on 26.09.23.
//

#ifndef TIXCLOCK_WLAN_H
#define TIXCLOCK_WLAN_H

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

class WLan {
public:
    static String ssid;
    static String password;
    static void init();
};


#endif //TIXCLOCK_WLAN_H
