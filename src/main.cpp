/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by pcbreflux
   Modified to work with NimBLE
   Modified for V2 by Chreece
   Modified by countrysideboy: Code cleanups, Chop Data, Encryption
   BLE advertisement format from https://bthome.io/

*/

#include "BTHome.h"

#define DEVICE_NAME "DIY-sensor" // The name of the sensor
BTHome bthome;
int advCount;
int numClicks = 0;

#if defined(BUTTON_PIN)
#include "OneButton.h"
OneButton button(BUTTON_PIN, true, true); // Button pin, active low, pullup enabled

void singleClick() {
    Serial.println("singleClick() detected.");
    numClicks = 1;
}

void doubleClick() {
    Serial.println("doubleClick() detected.");
    numClicks = 2;
}

void multiClick() {
    int n = button.getNumberClicks();
    Serial.printf("clicks = %d\n", n);
    numClicks = n;
}
#endif

void setup() {
    Serial.begin(115200);
#if defined(BUTTON_PIN)
    button.attachClick(singleClick);
    button.attachDoubleClick(doubleClick);
    button.attachMultiClick(multiClick);
#endif

    Serial.println("Creating the BTHome BLE device...");
    bthome.begin(DEVICE_NAME, false, "", true);

    // Seed the random number generator
    randomSeed(analogRead(0));
}

void loop() {
#if defined(BUTTON_PIN)
    button.tick();
#endif

    if (!bthome.isAdvertising()) {
        advCount++;
        // bthome.stop();
        bthome.resetMeasurement();

        if (advCount & 7 != 0) {
            // Add random variations
            float currentVoltage = 11.0 + (random(-200, 201) / 100.0f);
            float currentCurrent = 1.2 + (random(-100, 101) / 100.0f);
            float currentSpeed = (random(-100, 101) / 50.0f) + 0.5f;
            float currentAccel = (random(-100, 101) / 500.0f) + 0.5f;
            float currentElevation = 213.0 + (random(-100, 101) / 100.0f);

            bthome.addMeasurement(ID_DISTANCEM, currentElevation);
            bthome.addMeasurement(ID_SPD_SIGNED, currentSpeed);
            bthome.addMeasurement(ID_ACCELERATION_SIGNED, currentAccel);
            bthome.addMeasurement(ID_VOLTAGE, currentVoltage);
            bthome.addMeasurement(ID_CURRENT, currentCurrent);
        } else {
            uint64_t payload = 0x00000001; // manual example
            bthome.addMeasurement(DEVICEINFO_TYPEID, payload);
            payload = 0x04020100;
            bthome.addMeasurement(DEVICEINFO_FW4, payload);
            payload = 0x00060100;
            bthome.addMeasurement(DEVICEINFO_FW3,payload);
        }
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
        bthome.start(1);
    }
    yield();
}
