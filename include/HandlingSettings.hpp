#ifndef HANDLING_SETTINGS_HPP
#define HANDLING_SETTINGS_HPP

#include <iostream>

struct HandlingSettings {
  int gravityFrames;
  int softDropFrames;
  int maxLockDelayFrames;
  int maxLockDelayResets;
  int das;

  friend std::ostream& operator<<(std::ostream& o,
                                  const HandlingSettings& settings) {
    o << settings.gravityFrames << ' ';
    o << settings.softDropFrames << ' ';
    o << settings.maxLockDelayFrames << ' ';
    o << settings.maxLockDelayResets << ' ';
    o << settings.das << '\n';
    return o;
  }

  friend std::istream& operator>>(std::istream& i, HandlingSettings& settings) {
    i >> settings.gravityFrames;
    i >> settings.softDropFrames;
    i >> settings.maxLockDelayFrames;
    i >> settings.maxLockDelayResets;
    i >> settings.das;
    return i;
  }
};

#endif
