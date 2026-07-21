# esp_config.json — Configuration Reference

Place an `esp_config.json` file in the root directory of the SD card to configure the device at runtime.
All parameters are optional. If a parameter is missing or the file does not exist, the default value listed below is used.

## Wi-Fi Access Point

| Key          | Type   | Default            | Description                                                                 |
|--------------|--------|--------------------|-----------------------------------------------------------------------------|
| `ssid`       | string | `ESPSoundPlayer`   | SSID of the Wi-Fi access point created by the device.                       |
| `apPassword` | string | *(empty)*          | Password for the access point. If not set or empty, the AP is open (no authentication). |

## I2S / DAC audio output

| Key        | Type    | Default                  | Description                                           |
|------------|---------|--------------------------|-------------------------------------------------------|
| `i2sBclk`  | integer | *(board default)*        | GPIO pin for the I2S bit clock (BCLK).                |
| `i2sLrc`   | integer | *(board default)*        | GPIO pin for the I2S word select / left-right clock (LRC / WS). |
| `i2sDout`  | integer | *(board default)*        | GPIO pin for the I2S data output (DOUT).              |

If none of the three I2S keys are present, the audio library's built-in default pins are used and no `SetPinout` call is made.

Alternative key names `bclk`, `wclk`, and `dout` are also accepted for backwards compatibility.

## LED ring (WS2812B)

| Key                | Type    | Default | Description                                                   |
|--------------------|---------|---------|---------------------------------------------------------------|
| `ledRingEnabled`   | boolean | `false` | Set to `true` to enable the LED ring.                         |
| `ledRingPin`       | integer | `10`    | GPIO pin connected to the LED ring data input.                |
| `ledRingLedCount`  | integer | `12`    | Number of LEDs in the ring.                                   |
| `ledRingBrightness`| integer | `64`    | Overall brightness of the ring (0 – 255).                     |

## Example

```json
{
    "ssid": "MyESPPlayer",
    "apPassword": "secret123",
    "i2sLrc": 0,
    "i2sBclk": 1,
    "i2sDout": 2,
    "ledRingEnabled": true,
    "ledRingPin": 10,
    "ledRingLedCount": 12,
    "ledRingBrightness": 64
}
```

## Board-specific profiles

The `ESPConfig/` sub-folders each contain a ready-made `esp_config.json` for a specific board,
together with a wiring README that explains how to connect the components.

| Folder                       | Board                         |
|------------------------------|-------------------------------|
| `nologo_esp32c3_super_mini/` | nologo ESP32-C3 Super Mini    |
