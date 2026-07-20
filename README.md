![SleepUINO_Logo_PreDev](https://user-images.githubusercontent.com/48091357/111156537-25298a00-8596-11eb-8726-1fe5cd7bed93.png)

# ESP8266 Sound Player
## What you need
![SETUP](https://user-images.githubusercontent.com/48091357/134401159-6307f055-6e4c-4fda-816b-2a988379b693.gif)
1. Mini Breadbord 8,5 CM x 5,5 CM 400 pins DIY (up to US $1.43)
2. Jumper Wire (up to US $0.76)
3. WeMos D1 Mini PRO V3 (up to US $1.37)
4. MAX98357A I2S 3W Class D DAC Decoder Audio (up to US $2.53)
5. Micro SD Memory Shield Modul SPI (up to US $0.83) or a [modified SD Adapter](https://github.com/hredan/ESP_Sound_Player#replace-sd-card-reader-by-modified-sd-adapter) ($0.00)

The price examples comes from AliExpress. You will find the hardware also on Ebay, Amazon or by local hardware dealers.

## Build up
![BuildUp_GIF](https://user-images.githubusercontent.com/48091357/134401260-bcce7169-7a8a-4b67-bf06-51909ca59b8d.gif)
1. Build up, see [Circuit Diagram](https://github.com/hredan/ESP_Sound_Player/blob/main/README.md#circuit-diagram)
1. Flash the ESP Memory
1. Connect it with a speaker
1. Open the WLAN SSID ESPSoundPlayer
1. After this the Web Interface will start on your mobile device

## Optional: WS2812B LED ring (audio reactive)
The project can drive a WS2812B ring that reacts to the current MP3 output level.

Default settings are in `Config.h`:
- `LED_RING_ENABLED = true`
- `LED_RING_PIN = 5` (D1)
- `LED_RING_LED_COUNT = 12`
- `LED_RING_BRIGHTNESS = 64`

Wiring notes:
- Use a separate 5V supply for the LED ring.
- Connect all grounds together (ESP8266, DAC, LED power supply).
- Add a 330 Ohm resistor in series with the data line.
- A logic level shifter (3.3V to 5V) is recommended for robust operation.

# Circuit Diagram
![ESP Sound Player Breadboard](https://github.com/hredan/ESP_SoundBox/blob/main/CircuitDiagram/ESP8266_Audio_Player_Steckplatine.png)


![ESP Sound Player Circuit Diagram](https://github.com/hredan/ESP_SoundBox/blob/main/CircuitDiagram/ESP8266_Audio_Player_Schaltplan.png)

# Replace SD Card Reader by modified SD Adapter
It is also possible to use an SD Card Adapter. You have only add pins to the adapter an then you can use it on the breadboard.
*If you use a modified SD Adapter a supply voltage of 3.3V is needed instead the 5V used by the SD Card Reader in the circuit diagram!*

![Modified SD Adapter](https://github.com/hredan/ESP_SoundBox/blob/main/CircuitDiagram/Modifed_SD_Adapter.png)

A link to the Label you can find ![here](https://github.com/hredan/ESP_SoundBox/blob/main/CircuitDiagram/SD_AdapterLabel.pdf).

# Disclaimer
All this code is released under the GPL, and all of it is to be used at your own risk. If you find any bugs, please let me know via the GitHub issue tracker or drop me an email ([hredan@sleepuino.info](mailto:hredan@sleepuino.info)).
