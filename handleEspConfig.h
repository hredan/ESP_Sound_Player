#ifndef HANDLE_ESP_CONFIG_H_INCLUDED
#define HANDLE_ESP_CONFIG_H_INCLUDED

#include <Arduino.h>

#include "Config.h"

extern const char *ESP_CONFIG_FILE_NAME;

struct EspLedRingConfig
{
	bool enabled;
	uint8_t pin;
	uint16_t ledCount;
	uint8_t brightness;
};

struct EspApConfig
{
	String ssid;
	String password;
};

bool readEspConfigJson(String &configJson);
bool tryReadBoolFromJson(const String &json, const char *key, bool &value);
bool tryReadIntFromJson(const String &json, const char *key, int &value);
bool tryReadStringFromJson(const String &json, const char *key, String &value);
EspLedRingConfig getLedRingConfig();
EspApConfig getApConfig();

#endif // HANDLE_ESP_CONFIG_H_INCLUDED