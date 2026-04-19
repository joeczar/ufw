#include "UfwAudio.h"

UfwAudio::UfwAudio(HardwareSerial& serial) : serial_(serial) {}

bool UfwAudio::begin(uint32_t baud, Stream& debug, uint8_t startupVolume,
                     uint32_t initTimeoutMs, uint32_t initRetryMs,
                     uint32_t promptDelayMs) {
  serial_.begin(baud);

  const uint32_t startMs = millis();
  while (!player_.begin(serial_)) {
    debug.println("DFPlayer init failed, retrying...");
    delay(initRetryMs);

    if (millis() - startMs >= initTimeoutMs) {
      return false;
    }
  }

  debug.println("DFPlayer init OK");
  player_.switchFunction(player_.MUSIC);
  delay(promptDelayMs);
  player_.setPrompt(false);
  pause(&debug);
  const bool playModeOk = player_.setPlayMode(player_.SINGLE);
  player_.setVol(startupVolume);
  player_.enableAMP();

  debug.print("Set SINGLE mode: ");
  debug.println(playModeOk ? "OK" : "FAILED");
  debug.print("Play mode: ");
  debug.println(player_.getPlayMode());
  debug.print("Volume: ");
  debug.println(player_.getVol());

  return true;
}

bool UfwAudio::pause(Stream* debug) {
  const bool ok = player_.pause();
  if (debug != nullptr) {
    debug->println(ok ? "Pause command sent" : "Pause command failed");
  }
  return ok;
}

bool UfwAudio::playFileNum(uint16_t fileNumber, Stream* debug) {
  const bool ok = player_.playFileNum(fileNumber);
  if (debug != nullptr && !ok) {
    debug->println("playFileNum failed");
  }
  return ok;
}

bool UfwAudio::isPlaying() {
  return player_.isPlaying();
}

uint16_t UfwAudio::getTotalFile() {
  return player_.getTotalFile();
}

uint16_t UfwAudio::getCurFileNumber() {
  return player_.getCurFileNumber();
}

uint16_t UfwAudio::getCurTime() {
  return player_.getCurTime();
}

uint16_t UfwAudio::getTotalTime() {
  return player_.getTotalTime();
}

bool UfwAudio::copyFileName(char* buffer, size_t bufferSize) {
  if (buffer == nullptr || bufferSize == 0) {
    return false;
  }

  const String fileName = player_.getFileName();
  fileName.toCharArray(buffer, bufferSize);
  return buffer[0] != '\0';
}

uint8_t UfwAudio::getVolume() {
  return player_.getVol();
}

int UfwAudio::getPlayMode() {
  return player_.getPlayMode();
}
