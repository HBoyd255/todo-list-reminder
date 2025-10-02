/**
 * @file main.cpp
 * @brief A program to enable a flashing LED at 6am and 10pm, and disables it
 * once button 1 is pressed.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 2025-09-20
 * @copyright Copyright (c) 2025
 */

#include <WiFi.h>

#include "pinDefs_V2.h"
#include "time.h"
#include "wifiCredentials.h"

const char* ssid = WIFI_NAME;
const char* password = WIFI_PASSWORD;

const char* ntpServer = "pool.ntp.org";

// Set LED on at boot.
volatile bool FlashRedLED = true;
volatile bool FlashGreenLED = false;

void IRAM_ATTR turnOffAllLEDs() {
    FlashRedLED = false;
    FlashGreenLED = false;
}

void setup() {
    Serial.begin(9600);

    WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println("Connected!");

    configTime(0, 0, ntpServer);
    setenv("TZ", "GMT0BST,M3.5.0/1,M10.5.0", 1);
    tzset();

    pinMode(LED_1, OUTPUT);
    digitalWrite(LED_1, 0);

    pinMode(LED_2, OUTPUT);
    digitalWrite(LED_2, 0);

    pinMode(LED_3, OUTPUT);
    digitalWrite(LED_3, 0);

    pinMode(BUT_1, INPUT);

    attachInterrupt(digitalPinToInterrupt(BUT_1), turnOffAllLEDs, RISING);
}

void loop() {
    struct tm timedata;
    static int lastHour = 0;

    if (!getLocalTime(&timedata)) {
        digitalWrite(LED_3, 1);
    }

    // Hour Changed
    if (lastHour != timedata.tm_hour) {
        // At 6:00 AM everyday.
        if (timedata.tm_hour == 6) {
            FlashRedLED = true;
        }

        // At 10:00 PM everyday.
        if (timedata.tm_hour == 22) {
            FlashRedLED = true;
        }

        // At 12:00 PM every Wednesday and Thursday.
        if ((timedata.tm_hour == 12 && timedata.tm_wday == 3) ||
            (timedata.tm_hour == 12 && timedata.tm_wday == 4)) {
            FlashGreenLED = true;
        }
    }

    lastHour = timedata.tm_hour;

    // Create a boolean that will toggle on and off each seccond.
    bool flashingCoefficient = (millis() >> 9) & 1;

    // If the global FlashRedLED boolean is true, flash the LED.
    digitalWrite(LED_1, flashingCoefficient && FlashRedLED);

    // If the global FlashGreenLED boolean is true, flash the LED.
    digitalWrite(LED_2, !flashingCoefficient && FlashGreenLED);

    delay(10);
}