//
// Created by marcel on 26.09.23.
//

#include "WLan.h"


String WLan::ssid="MAKK";
String WLan::password="DieHasisWohnenNunInDessau";

void WLan::init() {
    Serial.println("Verbinde mit WLAN");
    WiFi.begin(ssid.c_str(), password.c_str());

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
