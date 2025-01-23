#ifndef HANDLING_SETTINGS_HPP
#define HANDLING_SETTINGS_HPP

#include <iostream>

struct HandlingSettings {
  int gravityFrames;
  int softDropFrames;
  int maxLockDelayFrames;
  int maxLockDelayResets;
  int das;

  friend std::ostream& operator<<(std::ostream&, const HandlingSettings&);
  friend std::istream& operator>>(std::istream&, HandlingSettings&);
};

#endif 
