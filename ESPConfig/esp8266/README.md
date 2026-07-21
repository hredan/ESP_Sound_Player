# ESP8266 Wiring Notes

For the ESP8266 setup, the SD card chip select pin is moved to GPIO 16.

Reason: the default SD card chip select pin conflicts with the I2S BCLK pin. Because of this collision, the `SetPinout` function does not work reliably on the ESP8266 when the default SD card pin is used.

Using GPIO 16 for the SD card chip select avoids that conflict, so the I2S interface can continue to use its default pin settings without problems.
