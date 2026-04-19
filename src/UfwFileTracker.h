#ifndef UFW_FILE_TRACKER_H
#define UFW_FILE_TRACKER_H

#include <Arduino.h>

class UfwFileTracker {
 public:
  static constexpr uint16_t MAX_ENTRIES = 64;
  static constexpr size_t MAX_NAME_LENGTH = 64;

  UfwFileTracker();

  void clear();
  void normalize(const char* name, char* normalized,
                 size_t normalizedSize) const;
  bool hasSeen(const char* normalizedName) const;
 bool remember(const char* normalizedName);
  uint16_t count() const;

 private:
  char seenNames_[MAX_ENTRIES][MAX_NAME_LENGTH];
  uint16_t seenCount_;
};

#endif
