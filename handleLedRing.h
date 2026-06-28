#ifndef HANDLE_LED_RING_H_INCLUDED
#define HANDLE_LED_RING_H_INCLUDED

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class HandleLedRing
{
  public:
    HandleLedRing(uint16_t ledCount, uint8_t dataPin, uint8_t brightness);
    void begin();
    void updateFromAudioLevel(float level, bool isSoundPlaying);

  private:
    Adafruit_NeoPixel _strip;
    uint16_t _ledCount;
    uint32_t _lastUpdate;
    float _visualLevel;

    uint32_t colorFromPosition(float position) const;
};

#endif // HANDLE_LED_RING_H_INCLUDED
