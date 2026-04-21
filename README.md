# BTHomeV2 ESP32 Example for Arduino 3.x

An BTHome v2 example.
Make your DIY sensor, BTHome compatible.

This is an example for a DIY [BTHome v2](https://bthome.io/) sensor.

The header file contains human readable variables for the hex Object ids.

Sending entities to Home Assistant is simplified.

1) Download the files from Arduino Code folder.
1) Edit the BTHome.ino in your favourite Arduino IDE
1) Change the DEVICE_NAME to the one that should be recognised in BTHome integration
1) Change the BIND_KEY with your preference or remove it along with the `#define ENABLE_ENCRYPT` for no encryption
1) Add the object id of the individual entities and the state of it as in the examples
1) Compile and upload to ESP32

Read the comments in the code for more info.
For the object ids consider using the variables from BTHome.h instead.

## Build Targets

This project supports multiple ESP32 boards via PlatformIO environments:

| Target                     | Board              | MCU      | Notes                                 |
| -------------------------- | ------------------ | -------- | ------------------------------------- |
| `atom-s3u`                 | ESP32-S3 DevKit    | ESP32-S3 | USB CDC, Button on pin 41             |
| `atom-s3`                  | ESP32-S3 DevKit    | ESP32-S3 | USB CDC, Button on pin 41             |
| `m5stamp-c3u`              | M5Stamp C3U        | ESP32-C3 | USB CDC, Button on pin 9, 160MHz      |
| `m5stick-c`                | M5Stick-C          | ESP32    | Uses Nimble-Arduino, Button on pin 37 |
| `esp32p4_waveshare_devkit` | Waveshare ESP32-P4 | ESP32-P4 | Debug build                           |
| `m5stack_nanoc6`           | M5Stack NanoC6     | ESP32-C6 | USB CDC enabled                       |

Build with: `platformio run --environment <target>`

Upload with: `platformio run --target upload --environment <target> --upload-port /dev/ttyUSB0`

## Looking for a BTHomeV2 decoder?

see https://github.com/mhaberler/BTHomeDecoder

## History

Original work: https://github.com/TheDigital1/ESP32_BTHome.git

forked by: https://github.com/Chreece/BTHomeV2-ESP32-example.git (unmaintained)

forked by and migrated to Nimble-Arduino: https://github.com/Cuddon/BTHomeV2-ESP32-example.git with encryption support and cleanups by [@countrysideboy](https://github.com/countrysideboy)

I forked the latter and made Nimble-Arduino optional as Arduino 3.0 has sufficient BLE support (see the `m5stick-c` target which still uses Nimble-Arduino).

I also added Pioarduino Platformio-style ini support.

Encryption is untested in this fork.