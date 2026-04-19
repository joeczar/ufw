#ifndef UFW_LED_H
#define UFW_LED_H

#include <Arduino.h>

class UfwLed {
 public:
  UfwLed(const uint8_t* animationPins, size_t animationPinCount,
         int statusPin = -1, int builtinPin = LED_BUILTIN);

  void begin();
  void clear();
  void setPin(uint8_t pin, bool on);
  void setStatus(bool on);
  void pulseBootPattern(uint8_t pulses = 3, uint16_t onMs = 100,
                        uint16_t offMs = 100);
  void startFillSweep(uint16_t stepDelayMs);
  void startFadeSweep(uint16_t frameDelayMs, uint8_t brightnessStep = 16);
  void stopAnimation();
  void update();
  bool isAnimating() const;

 private:
  enum AnimationPhase {
    kIdle,
    kFillOn,
    kFillOff,
    kFadeUp,
    kFadeDown,
  };

  bool hasAnimationPin(uint8_t pin) const;
  void writeStatusPins(bool on);
  void writeAllAnimationPins(bool on);

  const uint8_t* animationPins_;
  size_t animationPinCount_;
  int statusPin_;
  int builtinPin_;
  AnimationPhase animationPhase_;
  size_t animationIndex_;
  uint16_t stepDelayMs_;
  uint8_t brightnessStep_;
  int brightness_;
  uint32_t lastStepMs_;
};

#endif
