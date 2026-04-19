#include "UfwFileTracker.h"

#include <ctype.h>
#include <string.h>

UfwFileTracker::UfwFileTracker() : seenCount_(0) {
  clear();
}

void UfwFileTracker::clear() {
  seenCount_ = 0;
  for (uint16_t i = 0; i < MAX_ENTRIES; ++i) {
    seenNames_[i][0] = '\0';
  }
}

void UfwFileTracker::normalize(const char* name, char* normalized,
                               size_t normalizedSize) const {
  if (name == nullptr || normalized == nullptr || normalizedSize == 0) {
    return;
  }

  size_t writeIndex = 0;
  for (size_t i = 0; name[i] != '\0' && writeIndex + 1 < normalizedSize; ++i) {
    const unsigned char c = static_cast<unsigned char>(name[i]);
    if (isspace(c)) {
      continue;
    }
    normalized[writeIndex++] = static_cast<char>(toupper(c));
  }
  normalized[writeIndex] = '\0';
}

bool UfwFileTracker::hasSeen(const char* normalizedName) const {
  if (normalizedName == nullptr || normalizedName[0] == '\0') {
    return false;
  }

  for (uint16_t i = 0; i < seenCount_; ++i) {
    if (strcmp(seenNames_[i], normalizedName) == 0) {
      return true;
    }
  }
  return false;
}

bool UfwFileTracker::remember(const char* normalizedName) {
  if (normalizedName == nullptr || normalizedName[0] == '\0' ||
      hasSeen(normalizedName) || seenCount_ >= MAX_ENTRIES) {
    return false;
  }

  strncpy(seenNames_[seenCount_], normalizedName, MAX_NAME_LENGTH - 1);
  seenNames_[seenCount_][MAX_NAME_LENGTH - 1] = '\0';
  ++seenCount_;
  return true;
}

uint16_t UfwFileTracker::count() const {
  return seenCount_;
}
