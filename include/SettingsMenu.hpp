#ifndef SETTINGS_MENU_HPP
#define SETTINGS_MENU_HPP

#include "HandlingSettings.hpp"
#include <utility>

enum class Resolution { Small, Medium, Big, FullScreen };
std::pair<int, int> resolutionPair(Resolution resolution);

class SettingsMenu {
  static Resolution resolution;
  static HandlingSettings handlingSettings; 
  int selectedOption = 0;

  void resizeScreenHigher();
  void resizeScreenLower();

  public:
  ~SettingsMenu();
  void draw() const;
  void update();
  bool shouldStopRunning() const;
  static Resolution getResolution();
  static const HandlingSettings& getHandlingSettings();
};

#endif
