/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by pcbreflux
   Modified to work with NimBLE
   Modified for V2 by Chreece
   Modified by countrysideboy: Code cleanups, Chop Data, Encryption
   BLE advertisement format from https://bthome.io/

*/

#include "BTHome.h"

#define DEVICE_NAME "DIY-sensor"  // The name of the sensor
// #define DEVICE_NAME_LENGTH (sizeof(DEVICE_NAME) - 1)

// #define ENABLE_ENCRYPT // Remove this line for no encryption

// Change the bind key any string of 32 hex characters (a-f, 0-9).
// The Home Assistant BTHome integration will autodiscover your device and will ask you to enter this bind key.
String BIND_KEY = "431d39c1d7cc1ac1aef224cd096db934";

// Create a global instance of the BTHome class
BTHome bthome;

void setup() {
    Serial.begin(115200);
    delay(1000);

    log_i("Creating the BTHome BLE device..");
    bthome.packetId = 42;
#ifdef ENABLE_ENCRYPT
    bthome.begin(DEVICE_NAME, true, BIND_KEY, false);
#else
    bthome.begin(DEVICE_NAME, false, "", false, true, true);
#endif
}

void loop() {

    bthome.packetId++;

    // bthome.setDeviceName(DEVICE_NAME);
    //MEASUREMENT_MAX_LEN = 23, ENABLE_ENCRYPT will use extra 8 bytes, so each Measurement should smaller than 15
#if 1
    // 1st method: just addMeasurement as much as you can, the code will split and send the adv packet automatically
    // each adv packet sending lasts for 1500ms
    bthome.resetMeasurement();
    // bthome.addMeasurement(sensorid, value) you can use the sensorids from the BTHome.h file
    // *** The Object ids of addMeasurement have to be applied in numerical order (from low to high) in your advertisement ***
    bthome.addMeasurement(ID_TEMPERATURE_PRECISE, 35.00f);//3
#endif

#if 1
    bthome.addMeasurement(ID_HUMIDITY_PRECISE, 40.00f);//3
    bthome.addMeasurement(ID_PRESSURE, 1023.86f);//4
    bthome.addMeasurement(ID_ILLUMINANCE, 50.81f);//4 bytes
    bthome.addMeasurement_state(STATE_POWER_ON, STATE_ON);//2
    bthome.addMeasurement(ID_CO2, (uint64_t)1208);//3
    bthome.addMeasurement(ID_TVOC, (uint64_t)350);//3

    bthome.addMeasurement_state(EVENT_BUTTON, EVENT_BUTTON_PRESS);//2 button press
    bthome.addMeasurement_state(EVENT_DIMMER, EVENT_DIMMER_RIGHT, 6); //3, rotate right 6 steps
#endif
    // TEXT data
    String msg = "BLABLAddadasdasXYZ";
    bthome.addMeasurement(ID_TEXT, (uint8_t *)msg.c_str(), msg.length());

    // RAW data
    uint8_t raw[] = "123";
    bthome.addMeasurement(ID_RAW, raw, sizeof(raw)-1);

    // bthome.sendPacket();
    // bthome.buildPacket();

    if (bthome.isAdvertising()) {
        bthome.stop();
    }
    bthome.buildPacket();
    bthome.start(2000);

    delay(5000);
    bthome.stop();
    delay(1000);

    // bthome.resetMeasurement();



#if 0
    // 2nd method: make sure each measurement data length <=15 and start(stop) manually
    bthome.resetMeasurement();
    bthome.addMeasurement(ID_PACKET, (uint64_t)packetId++);
    bthome.addMeasurement(ID_TEMPERATURE_PRECISE, 26.00f);//3
    bthome.addMeasurement(ID_HUMIDITY_PRECISE, 70.00f);//3
    bthome.addMeasurement(ID_PRESSURE, 1000.86f);//4
    bthome.addMeasurement(ID_ILLUMINANCE, 1008.81f);//4 bytes
    bthome.buildPacket();
    bthome.start();//start the first adv data
    delay(1500);

    bthome.resetMeasurement();
    bthome.addMeasurement_state(STATE_POWER_ON, STATE_OFF);//2
    bthome.addMeasurement(ID_CO2, (uint64_t)458);//3
    bthome.addMeasurement(ID_TVOC, (uint64_t)220);//3
    bthome.addMeasurement_state(EVENT_BUTTON, EVENT_BUTTON_PRESS);//2, button press
    bthome.addMeasurement_state(EVENT_DIMMER, EVENT_DIMMER_RIGHT, 6); //3, rotate right 6 steps
    bthome.buildPacket();//change the adv data
    delay(1500);
    bthome.stop();
#endif
}
