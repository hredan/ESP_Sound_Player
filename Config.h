#ifndef CONFIG_H_
#define CONFIG_H_

// define constant values
const String CONFIG_FILE_NAME = "/saveConfig.json";

// WS2812B ring fallback configuration
const bool LED_RING_ENABLED = false;
const uint8_t LED_RING_PIN = 10;
const uint16_t LED_RING_LED_COUNT = 12;
const uint8_t LED_RING_BRIGHTNESS = 64;

const char DEFAULT_AP_SSID[] = "ESPSoundPlayer";

#endif  // CONFIG_H_
