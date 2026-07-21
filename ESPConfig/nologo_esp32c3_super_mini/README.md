# ESP32-C3 Super Mini Wiring

This folder contains the runtime pin configuration for the `nologo_esp32c3_super_mini` board.

All external components in this setup require a 5 V supply and can be powered directly from the ESP32-C3 Super Mini 5 V pin.

## ASCII pinout

```text
						 ESP32-C3 Super Mini

                             USB
                             ||
            MISO      GPIO5 [  ] 5V
            MOSI      GPIO6 [  ] GND
              SS      GPIO7 [  ] 3V3 
                      GPIO8 [  ] GPIO4   SCK
                      GPIO9 [  ] GPIO3
LED ring data in ->   GPIO10[  ] GPIO2   i2sDout
                      GPIO20[  ] GPIO1   i2sBclk
                      GPIO21[  ] GPIO0   i2sLrc

I2S / DAC wiring used by this profile:
- GPIO1  -> I2S BCLK
- GPIO0  -> I2S LRC / WS
- GPIO2  -> I2S DOUT

Power wiring used by this profile:
- 5V     -> DAC VCC
- 5V     -> LED ring VCC
- GND    -> DAC GND
- GND    -> LED ring GND
```

## Components

### DAC / I2S audio output

The DAC is connected through the I2S output pins defined in `esp_config.json`:

- `i2sBclk`: GPIO 1
- `i2sLrc`: GPIO 0
- `i2sDout`: GPIO 2

These pins are used by the audio output driver for bit clock, word select / left-right clock, and data out.

The DAC module should also be connected to the board's 5 V and GND pins.

### LED ring

The LED ring uses the following configuration:

- `ledRingEnabled`: `true`
- `ledRingPin`: GPIO 10
- `ledRingLedCount`: 12
- `ledRingBrightness`: 64

This means a 12-pixel WS2812B compatible LED ring is connected to GPIO 10.

The LED ring should also be connected to the board's 5 V and GND pins.

### SD card

The SD card uses the board's default SPI / SD wiring.

No custom SD card pins are configured in `esp_config.json` for this board.
