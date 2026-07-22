#include <SD.h>
#include <ctype.h>

#include "./handle_esp_config.h"

const char* ESP_CONFIG_FILE_NAME = "/esp_config.json";

namespace {
EspLedRingConfig buildDefaultLedRingConfig() {
  EspLedRingConfig config;
  config.enabled = LED_RING_ENABLED;
  config.pin = LED_RING_PIN;
  config.ledCount = LED_RING_LED_COUNT;
  config.brightness = LED_RING_BRIGHTNESS;
  return config;
}

EspApConfig buildDefaultApConfig() {
  EspApConfig config;
  config.ssid = DEFAULT_AP_SSID;
  config.password = "";
  return config;
}
}  // namespace

bool readEspConfigJson(String& configJson) {
  if (!SD.exists(ESP_CONFIG_FILE_NAME)) {
    return false;
  }

  File configFile = SD.open(ESP_CONFIG_FILE_NAME, FILE_READ);
  if (!configFile) {
    return false;
  }

  configJson = configFile.readString();
  configFile.close();
  return true;
}

bool tryReadBoolFromJson(const String& json, const char* key, bool& value) {
  String token = "\"" + String(key) + "\"";
  int keyPos = json.indexOf(token);
  if (keyPos < 0) {
    return false;
  }

  int colonPos = json.indexOf(':', keyPos + token.length());
  if (colonPos < 0) {
    return false;
  }

  int valuePos = colonPos + 1;
  while (valuePos < json.length() && isspace(json.charAt(valuePos))) {
    valuePos++;
  }

  if (json.startsWith("true", valuePos)) {
    value = true;
    return true;
  }

  if (json.startsWith("false", valuePos)) {
    value = false;
    return true;
  }

  if (valuePos < json.length() && json.charAt(valuePos) == '1') {
    value = true;
    return true;
  }

  if (valuePos < json.length() && json.charAt(valuePos) == '0') {
    value = false;
    return true;
  }

  return false;
}

bool tryReadIntFromJson(const String& json, const char* key, int& value) {
  String token = "\"" + String(key) + "\"";
  int keyPos = json.indexOf(token);
  if (keyPos < 0) {
    return false;
  }

  int colonPos = json.indexOf(':', keyPos + token.length());
  if (colonPos < 0) {
    return false;
  }

  int valuePos = colonPos + 1;
  while (valuePos < json.length() && isspace(json.charAt(valuePos))) {
    valuePos++;
  }

  if (valuePos < json.length() && json.charAt(valuePos) == '"') {
    valuePos++;
  }

  int endPos = valuePos;
  if (endPos < json.length() &&
      (json.charAt(endPos) == '-' || json.charAt(endPos) == '+')) {
    endPos++;
  }

  bool hasDigit = false;
  while (endPos < json.length() && isdigit(json.charAt(endPos))) {
    hasDigit = true;
    endPos++;
  }

  if (!hasDigit) {
    return false;
  }

  value = json.substring(valuePos, endPos).toInt();
  return true;
}

bool tryReadStringFromJson(const String& json, const char* key, String& value) {
  String token = "\"" + String(key) + "\"";
  int keyPos = json.indexOf(token);
  if (keyPos < 0) {
    return false;
  }

  int colonPos = json.indexOf(':', keyPos + token.length());
  if (colonPos < 0) {
    return false;
  }

  int valuePos = colonPos + 1;
  while (valuePos < json.length() && isspace(json.charAt(valuePos))) {
    valuePos++;
  }

  if (valuePos >= json.length() || json.charAt(valuePos) != '"') {
    return false;
  }

  valuePos++;
  int endPos = valuePos;
  while (endPos < json.length() && json.charAt(endPos) != '"') {
    endPos++;
  }

  if (endPos >= json.length()) {
    return false;
  }

  value = json.substring(valuePos, endPos);
  return value.length() > 0;
}

EspLedRingConfig getLedRingConfig() {
  EspLedRingConfig config = buildDefaultLedRingConfig();
  String configJson;
  if (!readEspConfigJson(configJson)) {
    return config;
  }

  bool enabled = config.enabled;
  if (tryReadBoolFromJson(configJson, "ledRingEnabled", enabled)) {
    config.enabled = enabled;
  }

  int pin = config.pin;
  if (tryReadIntFromJson(configJson, "ledRingPin", pin) && pin >= 0) {
    config.pin = static_cast<uint8_t>(pin);
  }

  int ledCount = config.ledCount;
  if (tryReadIntFromJson(configJson, "ledRingLedCount", ledCount) &&
      ledCount > 0) {
    config.ledCount = static_cast<uint16_t>(ledCount);
  }

  int brightness = config.brightness;
  if (tryReadIntFromJson(configJson, "ledRingBrightness", brightness) &&
      brightness >= 0 && brightness <= 255) {
    config.brightness = static_cast<uint8_t>(brightness);
  }

  return config;
}

EspApConfig getApConfig() {
  EspApConfig config = buildDefaultApConfig();
  String configJson;
  if (!readEspConfigJson(configJson)) {
    return config;
  }

  String ssid = config.ssid;
  if (tryReadStringFromJson(configJson, "ssid", ssid)) {
    config.ssid = ssid;
  }

  String password;
  if (tryReadStringFromJson(configJson, "apPassword", password)) {
    config.password = password;
  }

  return config;
}
