#ifndef SETTINGS_MENU_HPP
#define SETTINGS_MENU_HPP

#include "Runnable.hpp"
#include "HandlingSettings.hpp"
#include <utility>

enum class Resolution { Small, Medium, Big, FullScreen };
std::pair<int, int> resolutionPair(Resolution resolution);

class SettingsMenu : public Runnable {
  static Resolution resolution;
  static HandlingSettings handlingSettings; 
  
  void resizeScreen();
  virtual void draw() const override final;
  virtual void update() override final;
  virtual bool shouldStopRunning() const override final;

  public:
  ~SettingsMenu();
  static Resolution getResolution();
  static const HandlingSettings& getHandlingSettings();
};

#endif
