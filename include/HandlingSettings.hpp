#ifndef HANDLING_SETTINGS_HPP
#define HANDLING_SETTINGS_HPP

#include <iostream>

struct HandlingSettings {
  unsigned int gravityFrames;
  unsigned int softDropFrames;
  unsigned int maxLockDelayFrames;
  unsigned char maxLockDelayResets;
  unsigned int das;

  friend std::ostream& operator<<(std::ostream&, const HandlingSettings&);
  friend std::istream& operator>>(std::istream&, HandlingSettings&);
};

#endif 
