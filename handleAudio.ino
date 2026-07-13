
#include "handleAudio.h"

HandleAudio::HandleAudio()
{
    _soundIsPlaying = false;
    audioLogger = &Serial;
   
    _out = new AudioOutputI2S();
    // #ifdef ESP32
    // Serial.printf("AudioOutputI2S.SetPinout %d\n", _out->SetPinout(1, 0, 2));
    // #endif

    _audioGen = new AudioGeneratorMP3();
    _source = new AudioFileSourceSD();
};

//default volume value = 2
float HandleAudio:: _maxGain = 0.1;
bool HandleAudio::_soundIsPlaying = false;

AudioGeneratorMP3 * HandleAudio::_audioGen = nullptr;
AudioFileSourceSD * HandleAudio::_source = nullptr;
AudioOutputI2S * HandleAudio::_out = nullptr;

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
