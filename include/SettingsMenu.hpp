#ifndef SETTINGS_MENU_HPP
#define SETTINGS_MENU_HPP

#include "HandlingSettings.hpp"
#include <utility>

enum class Resolution {
  Small,
  Medium,
  Big,
  FullScreen
};

std::pair<int, int> resolution_pair(Resolution resolution);

class SettingsMenu {
  static constexpr int OPTIONS = 3;
  int selected_option = 0;

public:
  ~SettingsMenu();
  void draw() const;
  void update();
  bool should_stop_running() const;

  struct Config {
    Resolution resolution;
    HandlingSettings handling_settings;
  };
  static const Config& config();
};

#endif
