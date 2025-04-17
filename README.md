# NPK Sensor Issues & UART on Arduino Nano 33 BLE Sense

---

## Having Trouble with NPK Sensor Readings?

**You're not alone!**

If you're getting incorrect readings from your NPK sensor, the issue is likely with the code used to handle responses from the sensor — especially if you're using code found online that does not use a proper Modbus library.

### The Problem

Many implementations:

- Send a request and assume the sensor's response is instantly available.
- Read bytes without checking if data is actually there.
- As a result:
  - Response for parameter 1 is missed (`0xFF` or `-1`).
  - Response for parameter 2 is actually for parameter 1.
  - Response for parameter 3 is actually for parameter 2.
  - The next request for parameter 1 reads the stale parameter 3 data.

This causes all values to be one step behind the actual request.

---

## Code Without Modbus Library

There’s a working example posted on the Arduino Forum that works well on an Arduino UNO:

[Working Code Without Modbus Library – Forum Post #14](https://forum.arduino.cc/t/max485-ttl-to-rs-485-modules-for-soil-npk-sensor/853077/115)

This version worked perfectly for me with an LCD display.

---

## Code With Modbus Library

Another implementation uses a proper Modbus library and is shared here:

[Code With Modbus Library – Forum Post #15](https://forum.arduino.cc/t/max485-ttl-to-rs-485-modules-for-soil-npk-sensor/853077/116)

This worked initially, but later stopped. Still troubleshooting why.

![Sensor Output](https://github.com/m-ogore/arduino_npk_sensor/assets/42109589/0f6f8593-ddfd-41e5-80ab-6a6a2bd9a44e)

---

## Arduino Nano 33 BLE Sense & UART

The `SoftwareSerial` library does not work on the Nano 33 BLE Sense due to the nRF52840 microcontroller (no pin change interrupts).  
Instead, use the `UART` class from the Mbed OS core.

### Sample Code

```
#include "mbed.h"
#include "Arduino.h"

// Create a UART object with your desired pins
// RX = pin 4, TX = pin 3 (you can change them)
UART mySerial(digitalPinToPinName(4), digitalPinToPinName(3), NC, NC);

void setup() {
  // Start built-in Serial for debugging
  Serial.begin(9600);
  while (!Serial);

  // Start the custom UART port
  mySerial.begin(9600);
  Serial.println("Custom UART initialized on pins 4 (RX) and 3 (TX)");
}

void loop() {
  // Echo data between Serial and mySerial
  if (mySerial.available()) {
    char c = mySerial.read();
    Serial.print("From UART: ");
    Serial.println(c);
  }

  if (Serial.available()) {
    char c = Serial.read();
    mySerial.write(c);
  }
}
```

![video](}


[Full video Presentation By Samson Ooko](https://www.youtube.com/watch?v=E8Quk76CivI)
