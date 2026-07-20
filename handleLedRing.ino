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

uint32_t HandleLedRing::colorFromAudioLevel(float level) const
{
    if (level < 0.0f)
    {
        level = 0.0f;
    }
    if (level > 1.0f)
    {
        level = 1.0f;
    }

    // Map low levels to blue/cyan and high levels to warm colors.
    uint8_t red = static_cast<uint8_t>(constrain((level - 0.25f) * 340.0f, 0.0f, 255.0f));
    uint8_t green = static_cast<uint8_t>(constrain((1.0f - fabsf(level - 0.5f) * 2.0f) * 255.0f, 0.0f, 255.0f));
    uint8_t blue = static_cast<uint8_t>(constrain((1.0f - level) * 255.0f, 0.0f, 255.0f));
    return _strip.Color(red, green, blue);
}

uint8_t HandleLedRing::brightnessFromAudioLevel(float level) const
{
    level = constrain(level, 0.0f, 1.0f);
    const uint8_t minBrightness = 24;
    const uint8_t maxBrightness = 255;
    return static_cast<uint8_t>(minBrightness + (maxBrightness - minBrightness) * level);
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
    _strip.setBrightness(brightnessFromAudioLevel(_visualLevel));

    uint32_t color = colorFromAudioLevel(_visualLevel);

    for (uint16_t i = 0; i < _ledCount; i++)
    {
        _strip.setPixelColor(i, color);
    }

    _strip.show();
}
