#ifndef HANDLE_AUDIO_H_INCLUDED
#define HANDLE_AUDIO_H_INCLUDED

//Audio functionality comes from:
//https://github.com/earlephilhower/ESP8266Audio

#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
//#include <i2s.h>
#include "AudioOutputI2S.h"

class ReactiveAudioOutputI2S : public AudioOutputI2S
{
  public:
    ReactiveAudioOutputI2S();
    bool ConsumeSample(int16_t sample[2]) override;
    float getLevel() const;

  private:
    float _level;
};


class HandleAudio{
  using CallBackSoundIsDone = void (*) ();
  
  public:
    HandleAudio();
    static void playSound(String filename, int volume);
    static void stopSound();
    static void setMaxGain(float maxGain);
    float getCurrentLevel();
    bool isSoundPlaying();
  
  private:
    static bool _soundIsPlaying;
    static float _maxGain;
  	static AudioGeneratorMP3 *_audioGen;
    static AudioFileSourceSD *_source;
    static ReactiveAudioOutputI2S *_out;
};
#endif // HANDLE_AUDIO_H_INCLUDED
