/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by pcbreflux
   Modified to work with NimBLE
   Modified for V2 by Chreece
   Modified by countrysideboy: Code cleanups, Chop Data, Encryption
   BLE advertisement format from https://bthome.io/

*/

#include "OneButton.h"
OneButton button(BUTTON_PIN, true, true); // Button pin, active low, pullup enabled

#include "BTHome.h"

#define DEVICE_NAME "DIY-sensor"  // The name of the sensor
BTHome bthome;

// Base values for temperature and humidity
float baseTemperature = 35.00f;
float baseHumidity = 40.00f;

int numClicks = 0;
void singleClick() {
    Serial.println("singleClick() detected.");
    numClicks = 1;
}

// this function will be called when the button was pressed 2 times in a short timeframe.
void doubleClick() {
    Serial.println("doubleClick() detected.");
    numClicks = 2;
}

void multiClick() {
    int n = button.getNumberClicks();
    Serial.printf("clicks = %d\n", n);
    numClicks = n;
}


void setup() {
    Serial.begin(115200);
    button.attachClick(singleClick);
    button.attachDoubleClick(doubleClick);
    button.attachMultiClick(multiClick);

    Serial.println("Creating the BTHome BLE device...");
    bthome.begin(DEVICE_NAME, false, "", true);

    // Seed the random number generator
    randomSeed(analogRead(0));
}

void loop() {
    button.tick();
    if (!bthome.isAdvertising()) {
        // bthome.stop();
        bthome.resetMeasurement();

        // Add random variations to the base values (±2.0 for temperature, ±5.0 for humidity)
        float currentTemperature = baseTemperature + (random(-200, 201) / 100.0f); // ±2.0°C
        float currentHumidity = baseHumidity + (random(-500, 501) / 100.0f);       // ±5.0%
        float currentVoltage = 11.0 + (random(-200, 201) / 100.0f);
        float currentCurrent = 1.2 + (random(-100, 101) / 100.0f);
        // bthome.addMeasurement(ID_TEMPERATURE_PRECISE, currentTemperature);
        // bthome.addMeasurement(ID_HUMIDITY_PRECISE, currentHumidity);
        bthome.addMeasurement(ID_VOLTAGE, currentVoltage);
        bthome.addMeasurement(ID_CURRENT, currentCurrent);
        uint64_t payload = 0x01020304; // is interpreted as 0x0304

        payload = 0x00000001; // manual example
        bthome.addMeasurement(DEVICEINFO_TYPEID, payload);

        // payload = 0x00010204; // wrong
        // payload = 0x04020100;

        // bthome.addMeasurement(DEVICEINFO_FW4, payload);

        // payload = 0x00060100;
        // bthome.addMeasurement(DEVICEINFO_FW3,payload);

        switch (numClicks) {
            default:
                break;
            case 1:
                bthome.addMeasurement_state(EVENT_BUTTON, EVENT_BUTTON_PRESS);
                numClicks = 0;
                break;
            case 2:
                bthome.addMeasurement_state(EVENT_BUTTON, EVENT_BUTTON_DOUBLE_PRESS);
                numClicks = 0;
                break;
            case 3:
                bthome.addMeasurement_state(EVENT_BUTTON, EVENT_BUTTON_TRIPLE_PRESS);
                numClicks = 0;
                break;
        }
        bthome.buildPacket();
        bthome.start(1500);
    }
    yield();
}
