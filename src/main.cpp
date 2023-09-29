#include <Arduino.h>
#include "Display.h"
#include "Clock.h"
#include "WLan.h"



WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;


// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

int pos=0;
int dir=1;

void setup() {
// write your initialization code here
    Serial.begin(38400);
    while (!Serial)
        ;

    Display::init();
    Clock::init();
    WLan::init();


// Initialize a NTPClient to get time
    timeClient.begin();
    // Set offset time in seconds to adjust for your timezone, for example:
    // GMT +1 = 3600
    // GMT +8 = 28800
    // GMT -1 = -3600
    // GMT 0 = 0
    timeClient.setTimeOffset(7200);
    timeClient.setPoolServerName("de.pool.ntp.org");
    while (!timeClient.update()) {
        timeClient.forceUpdate();
    }

}

void loop() {

    formattedDate = timeClient.getFormattedTime();
    Serial.println(formattedDate);
    int hours = timeClient.getHours();
    int minutes = timeClient.getMinutes();

    int n = hours / 10;
    int o = hours % 10;
    int m = minutes / 10;
    int p = minutes % 10;


    int pins[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    if(hours==0 && minutes==0){
        pos+=dir;
        if(pos>=NUMPIXELS){
            pos=NUMPIXELS-1;
            dir=-1*dir;
        }else if(pos<=0){
            pos=0;
            dir=-1*dir;
        }
        pins[pos]=1;
    }else {
        pos=0;
        dir=1;

        int np = 0;
        while (n>0 && np < n) {
            int of = random(0, 3);
            if (of > 2) {
                of = 2;
            }

            pins[of] = 1;

            np = 0;
            for (int i = 0; i < 3; i++) {
                np = np + pins[i];
            }
        }

        np = 0;
        while (o>0 && np < o) {
            Serial.println(np);
            int of = random(0, 10);
            if (of > 8) {
                of = 8;
            }

            pins[of + 3] = 1;

            np = 0;
            for (int i = 3; i < 12; i++) {
                np = np + pins[i];
            }
        }

        if (minutes == 55) {
            for (int i = 12; i < 17; i++) {
                pins[i] = 1;
            }
            pins[18] = 1;
            pins[20] = 1;
            pins[22] = 1;
            pins[24] = 1;
            pins[26] = 1;
        } else {
            np = 0;
            while (m>0 && np < m) {
                int of = random(0, 7);
                if (of > 5) {
                    of = 5;
                }

                pins[of + 12] = 1;

                np = 0;
                for (int i = 12; i < 18; i++) {
                    np = np + pins[i];
                }
            }
            np = 0;
            while (p>0 && np < p) {
                Serial.println(np);
                int of = random(0, 9);
   if (of > 8) {
                    of = 8;
                }
                pins[of + 18] = 1;

                np = 0;
                for (int i = 18; i < 27; i++) {
                    np = np + pins[i];
                }
            }
        }
    }
    pixels.clear(); // Set all pixel colors to 'off'
    // The first NeoPixel in a strand is #0, second is 1, all the way up
    // to the count of pixels minus one.
    for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
        if (pins[i] == 1) {
            if(minutes==0 && hours==0){
                pixels.setPixelColor(i, pixels.Color(random(150)+10,random(150)+10,random(150)+10));
            }else {
                if (i >=0 and i < 3) {
                    pixels.setPixelColor(i, pixels.Color(150, 0, 0));
                } else if (i >= 3 and i < 12) {
                    pixels.setPixelColor(i, pixels.Color(150, 150, 0));
                } else if (i >= 12 and i < 18) {
                    pixels.setPixelColor(i, pixels.Color(0, 0, 150));
                } else {
                    pixels.setPixelColor(i, pixels.Color(150, 0, 150));
                }
            }
        } else {
            pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        }


    }
    pixels.show();
// write your code here
if(hours==0 && minutes==0){
    delay(250);
}else {
    delay(60000);
}
}