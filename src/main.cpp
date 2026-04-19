/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by pcbreflux
   Modified to work with NimBLE
   Modified for V2 by Chreece
   Modified by countrysideboy: Code cleanups, Chop Data, Encryption
   BLE advertisement format from https://bthome.io/

*/

#include "BTHome.h"
#include <atomic>

#define DEVICE_NAME "DIY-sensor" // The name of the sensor
#define DURATION 1000 //mS

BTHome bthome;
int advCount;
std::atomic<int> numClicks{0};
std::atomic<bool> fixedOidSet{false};

#if defined(BUTTON_PIN)
#include "OneButton.h"
OneButton button(BUTTON_PIN, true, true); // Button pin, active low, pullup enabled

void singleClick() {
    Serial.println("singleClick() detected.");
    numClicks.store(1);
}

void doubleClick() {
    numClicks.store(2);
    bool expected = fixedOidSet.load();
    while (!fixedOidSet.compare_exchange_weak(expected, !expected)) {
        // retry until toggled (expected updated by compare_exchange_weak)
    }
    Serial.printf("doubleClick() detected - fixedOidSet=%s\n", fixedOidSet.load() ? "true":"false");
}

void multiClick() {
    int n = button.getNumberClicks();
    Serial.printf("clicks = %d\n", n);
    numClicks.store(n);
}

// Run the OneButton state machine in its own FreeRTOS task
static void buttonTask(void *pvParameters) {
    (void) pvParameters;
    for (;;) {
        button.tick();
        vTaskDelay(pdMS_TO_TICKS(10)); // scan every 10 ms
    }
}
#endif

void setup() {
    Serial.begin(115200);
#if defined(BUTTON_PIN)
    button.attachClick(singleClick);
    button.attachDoubleClick(doubleClick);
    button.attachMultiClick(multiClick);

    // Run button scanning in its own FreeRTOS task
    xTaskCreate(
        buttonTask,          // task function
        "buttonTick",       // name
        2048,                // stack size
        NULL,                // params
        1,                   // priority
        NULL                 // task handle
    );
#endif

    Serial.println("Creating the BTHome BLE device...");
    bthome.begin(DEVICE_NAME, false, "", true);

    // Seed the random number generator
    randomSeed(analogRead(0));
}

void loop() {
    if (!bthome.isAdvertising()) {
        advCount++;
        bthome.resetMeasurement();

        if ((advCount & 1 != 0) || fixedOidSet.load()) {
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
        int clicks = numClicks.exchange(0);
        switch (clicks) {
            default:
                break;
            case 1:
                bthome.addMeasurement_state(EVENT_BUTTON, EVENT_BUTTON_PRESS);
                break;
            case 2:
                bthome.addMeasurement_state(EVENT_BUTTON, EVENT_BUTTON_DOUBLE_PRESS);
                break;
            case 3:
                bthome.addMeasurement_state(EVENT_BUTTON, EVENT_BUTTON_TRIPLE_PRESS);
                break;
        }
        bthome.buildPacket();
        bthome.start(DURATION);
        delay(DURATION);
        bthome.stop();
    }
    yield();
}
