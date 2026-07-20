#include "handleLedRing.h"
#include <math.h>

HandleLedRing::HandleLedRing(uint16_t ledCount, uint8_t dataPin, uint8_t brightness)
    : _strip(ledCount, dataPin, NEO_GRB + NEO_KHZ800),
      _ledCount(ledCount),
      _lastUpdate(0),
      _visualLevel(0.0f)
{
    _strip.setBrightness(brightness);
}

void HandleLedRing::begin()
{
    _strip.begin();
    _strip.clear();
    _strip.show();
}

uint32_t HandleLedRing::colorFromPosition(float position) const
{
    if (position < 0.0f)
    {
        position = 0.0f;
    }
    if (position > 1.0f)
    {
        position = 1.0f;
    }

    // Gradient from green to red.
    uint8_t red = static_cast<uint8_t>(position * 255.0f);
    uint8_t green = static_cast<uint8_t>((1.0f - position) * 255.0f);
    return _strip.Color(red, green, 0);
}

void HandleLedRing::updateFromAudioLevel(float level, bool isSoundPlaying)
{
    uint32_t now = millis();
    if ((now - _lastUpdate) < 20)
    {
        return;
    }
    _lastUpdate = now;

    if (!isSoundPlaying)
    {
        _visualLevel *= 0.85f;
    }
    else
    {
        // Compression keeps visible movement in quiet passages.
        float compressed = sqrtf(constrain(level, 0.0f, 1.0f));
        if (compressed > _visualLevel)
        {
            _visualLevel += (compressed - _visualLevel) * 0.35f;
        }
        else
        {
            _visualLevel += (compressed - _visualLevel) * 0.12f;
        }
    }

    _visualLevel = constrain(_visualLevel, 0.0f, 1.0f);
    int activeLeds = static_cast<int>(_visualLevel * _ledCount + 0.5f);

    for (uint16_t i = 0; i < _ledCount; i++)
    {
        if (i < activeLeds)
        {
            float pos = _ledCount > 1 ? static_cast<float>(i) / (_ledCount - 1) : 0.0f;
            _strip.setPixelColor(i, colorFromPosition(pos));
        }
        else
        {
            _strip.setPixelColor(i, 0);
        }
    }

    _strip.show();
}
