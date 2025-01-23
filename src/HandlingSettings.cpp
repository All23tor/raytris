#include "HandlingSettings.hpp"
#include <ostream>

std::ostream& operator<<(std::ostream& o, const HandlingSettings& settings) {
  o << settings.gravityFrames << ' ';
  o << settings.softDropFrames << ' ';
  o << settings.maxLockDelayFrames << ' ';
  o << settings.maxLockDelayResets << ' ';
  o << settings.das << '\n';
  return o;
}

std::istream& operator>>(std::istream& i, HandlingSettings& settings) {
  i >> settings.gravityFrames; 
  i >> settings.softDropFrames;
  i >> settings.maxLockDelayFrames;
  i >> settings.maxLockDelayResets;
  i >> settings.das;
  return i;
}
