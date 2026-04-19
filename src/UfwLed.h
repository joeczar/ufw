#ifndef UFW_LED_H
#define UFW_LED_H

#include <Arduino.h>

#include "UfwTypes.h"

class UfwLed {
 public:
  UfwLed(const uint8_t* animationPins, size_t animationPinCount,
         int statusPin = -1, int builtinPin = LED_BUILTIN);
  template <size_t N>
  UfwLed(const uint8_t (&animationPins)[N], int statusPin = -1,
         int builtinPin = LED_BUILTIN)
      : UfwLed(animationPins, N, statusPin, builtinPin) {}

  void begin();
  void clear();
  void setPin(uint8_t pin, bool on);
  void setStatus(bool on);
  void pulseBootPattern(uint8_t pulses = 3, uint16_t onMs = 100,
                        uint16_t offMs = 100);
  void startFillSweep(uint16_t stepDelayMs, size_t maxLit = 0,
                      uint8_t brightness = 255);
  void startFillSweep(uint16_t stepDelayMs, size_t maxLit,
                      UfwBrightnessProfile brightnessProfile);
  template <size_t N>
  void startFillSweep(uint16_t stepDelayMs, size_t maxLit,
                      const uint8_t (&brightnessProfile)[N]) {
    startFillSweep(stepDelayMs, maxLit,
                   UfwBrightnessProfile{brightnessProfile, N});
  }
  void startFadeSweep(uint16_t frameDelayMs, size_t maxLit = 1,
                      uint8_t brightness = 255, uint8_t fadeStep = 16);
  void startFadeSweep(uint16_t frameDelayMs, size_t maxLit,
                      UfwBrightnessProfile brightnessProfile,
                      uint8_t fadeStep = 16);
  template <size_t N>
  void startFadeSweep(uint16_t frameDelayMs, size_t maxLit,
                      const uint8_t (&brightnessProfile)[N],
                      uint8_t fadeStep = 16) {
    startFadeSweep(frameDelayMs, maxLit,
                   UfwBrightnessProfile{brightnessProfile, N}, fadeStep);
  }
  void startComet(uint16_t frameDelayMs, size_t maxLit = 1,
                  uint8_t brightness = 255, uint8_t fadeStep = 16);
  void startComet(uint16_t frameDelayMs, size_t maxLit,
                  UfwBrightnessProfile brightnessProfile,
                  uint8_t fadeStep = 16);
  template <size_t N>
  void startComet(uint16_t frameDelayMs, size_t maxLit,
                  const uint8_t (&brightnessProfile)[N],
                  uint8_t fadeStep = 16) {
    startComet(frameDelayMs, maxLit,
               UfwBrightnessProfile{brightnessProfile, N}, fadeStep);
  }
  void startBreathe(uint16_t frameDelayMs, uint8_t brightness = 255,
                    uint8_t fadeStep = 16);
  void startBreatheOnce(uint16_t frameDelayMs, uint8_t brightness = 255,
                        uint8_t fadeStep = 16);
  void startBreatheCount(uint16_t frameDelayMs, uint16_t pulseCount,
                         uint8_t brightness = 255, uint8_t fadeStep = 16);
  void stopAnimation();
  void update();
  bool isAnimating() const;

 private:
  static constexpr size_t kMaxTrackedPins = 32;

  enum AnimationMode {
    kIdle,
    kFillSweep,
    kFadeSweep,
    kComet,
    kBreathe,
  };

  bool hasAnimationPin(uint8_t pin) const;
  size_t managedAnimationPinCount() const;
  size_t activeWindowSize() const;
  size_t maxSweepStep() const;
  uint8_t windowBrightness(size_t windowPosition) const;
  void resetBrightnessState();
  void startFillAnimation(uint16_t stepDelayMs, size_t maxLit,
                          uint8_t brightness,
                          UfwBrightnessProfile brightnessProfile);
  void setAllTargets(uint8_t brightness);
  void startWindowAnimation(AnimationMode mode, uint16_t frameDelayMs,
                            size_t maxLit, uint8_t brightness,
                            UfwBrightnessProfile brightnessProfile,
                            uint8_t fadeStep);
  void startBreatheCommon(uint16_t frameDelayMs, int pulseCount,
                          uint8_t brightness, uint8_t fadeStep);
  void refreshFadeTargets();
  bool fadeFrameSettled() const;
  void stepFadeFrame();
  void renderFadeFrame();
  void renderAnimationFrame();
  void writeAnimationPin(uint8_t pin, uint8_t brightness);
  void writeStatusPins(bool on);
  void writeAllAnimationPins(bool on);

  const uint8_t* animationPins_;
  size_t animationPinCount_;
  int statusPin_;
  int builtinPin_;
  AnimationMode animationMode_;
  size_t sweepStep_;
  size_t maxLit_;
  uint16_t stepDelayMs_;
  uint8_t brightness_;
  UfwBrightnessProfile brightnessProfile_;
  uint8_t brightnessStep_;
  uint32_t lastStepMs_;
  uint8_t breathePeakBrightness_;
  bool breatheIncreasing_;
  int breatheCyclesRemaining_;
  uint8_t currentBrightness_[kMaxTrackedPins];
  uint8_t targetBrightness_[kMaxTrackedPins];
};

#endif
