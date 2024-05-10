## NPK Sensor Issues

If your are reading this, YOU ARE NOT ALONE!!!!!

You are having problems with the wrong results coming back from the NPK sensor, here's why:

The issue is down to the awful code circulating around various websites that is used to read from one of those NPK sensors. Specifically in how the reading of the response from the sensor is handled.

The code transmits a request for the first parameter and then assumes that the response from the NPK sensor is instantly available as soon as the request is transmitted. The code then attempts to read the nonexistent response by blindly reading in the number of bytes expected to be available without carrying out any checks first. This results in the first request returning only 0xFF (actually -1 signaling no character available).

The code then transmits a request for the second parameter and once again reads in the expected number of response bytes without carrying out any checks. However, what is read is actually the response to the first parameter query.

Similarly when requesting the third parameter, what is read in is the response to the second parameter query.

The code then goes back and requests the first parameter and actually reads in the response to the third parameter.

That's why the values that are read back are out of step with the requests.

There is a bit of code I've posted at post #14 in the following discussion that should work for an UNO. It's no substitute for a proper Modbus library:

[Discussion Link](https://forum.arduino.cc/t/max485-ttl-to-rs-485-modules-for-soil-npk-sensor/853077/115)

### Code with Modbus Library
Credit to [LinktoModbus reply on arduino forum](https://forum.arduino.cc/t/max485-ttl-to-rs-485-modules-for-soil-npk-sensor/853077/116)

![Image](https://github.com/m-ogore/arduino_npk_sensor/assets/42109589/0f6f8593-ddfd-41e5-80ab-6a6a2bd9a44e)
