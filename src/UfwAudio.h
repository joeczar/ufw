#ifndef UFW_AUDIO_H
#define UFW_AUDIO_H

#include <Arduino.h>
#include <DFRobot_DF1201S.h>

class UfwAudio {
 public:
  explicit UfwAudio(HardwareSerial& serial);

  bool begin(uint32_t baud, Stream& debug, uint8_t startupVolume = 20,
             uint32_t initTimeoutMs = 15000, uint32_t initRetryMs = 1000,
             uint32_t promptDelayMs = 2000);
  bool pause(Stream* debug = nullptr);
  bool playFileNum(uint16_t fileNumber, Stream* debug = nullptr);
  bool isPlaying();
  uint16_t getTotalFile();
  uint16_t getCurFileNumber();
  uint16_t getCurTime();
  uint16_t getTotalTime();
  bool copyFileName(char* buffer, size_t bufferSize);
  uint8_t getVolume();
  int getPlayMode();

 private:
  HardwareSerial& serial_;
  DFRobot_DF1201S player_;
};

#endif
