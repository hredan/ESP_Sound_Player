
#include "handleAudio.h"

ReactiveAudioOutputI2S::ReactiveAudioOutputI2S() : _level(0.0f)
{
}

bool ReactiveAudioOutputI2S::ConsumeSample(int16_t sample[2])
{
    int32_t left = sample[0];
    int32_t right = sample[1];
    float peak = abs(left);
    if (abs(right) > peak)
    {
        peak = abs(right);
    }
    float normalized = peak / 32768.0f;

    // Fast attack and slower release for stable visualization.
    const float attack = 0.25f;
    const float release = 0.04f;
    if (normalized > _level)
    {
        _level += (normalized - _level) * attack;
    }
    else
    {
        _level += (normalized - _level) * release;
    }

    return AudioOutputI2S::ConsumeSample(sample);
}

float ReactiveAudioOutputI2S::getLevel() const
{
    return _level;
}

HandleAudio::HandleAudio()
{
    _soundIsPlaying = false;
    audioLogger = &Serial;
   
    _out = new ReactiveAudioOutputI2S();
    _audioGen = new AudioGeneratorMP3();
    _source = new AudioFileSourceSD();
};

//default volume value = 2
float HandleAudio:: _maxGain = 0.1;
bool HandleAudio::_soundIsPlaying = false;

AudioGeneratorMP3 * HandleAudio::_audioGen = nullptr;
AudioFileSourceSD * HandleAudio::_source = nullptr;
ReactiveAudioOutputI2S * HandleAudio::_out = nullptr;

float HandleAudio::getCurrentLevel()
{
    if (_out == nullptr)
    {
        return 0.0f;
    }
    return _out->getLevel();
}

void HandleAudio::setMaxGain(float maxGain)
{
    _maxGain = maxGain;
}

bool HandleAudio::isSoundPlaying()
{
    if (_audioGen->isRunning())
    {
        if (!_audioGen->loop()) 
        {
            _audioGen->stop();
            _soundIsPlaying = false;
            Serial.printf("MP3 done\n");
        }
    }
    return _soundIsPlaying;
}

void HandleAudio::stopSound()
{
    if (_audioGen->isRunning())
    {
        _audioGen->stop();
        _soundIsPlaying = false;
        Serial.println("stopSound() -> stop sound");
    }
}

void HandleAudio::playSound(String filename, int volume)
{
    Serial.printf("start playSound -> filename: %s, volume: %d\n", filename.c_str(), volume);
    filename = "/" + filename;
    if(SD.exists(filename))
    {
        if(_soundIsPlaying)
        {
            HandleAudio::stopSound();
        }
        //max gain value < 4
        float gain = volume * _maxGain / 100.0;
        if (gain >= 4.0)
        {
            gain = 3.99;
        }

        _out->SetGain(gain);
        Serial.println("use gain to play sound: " + String(gain));
        _source->open(filename.c_str());
        Serial.println("play sound");
        if (_audioGen->begin(_source, _out))
        {
            _soundIsPlaying = true;
        }
    }
    else
    {
        Serial.printf("Error file not found playSound -> filename: %s\n", filename.c_str());
    }
}
