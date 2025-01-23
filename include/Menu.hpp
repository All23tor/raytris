#ifndef MENU_H
#define MENU_H

#include "Runnable.hpp"
#include <utility>

enum class Resolution { Small, Medium, Big, FullScreen };

class Menu : public Runnable {
public:
  static constexpr int InitialWidth = 640;
  static constexpr int InitialHeight = 360;
  enum class Option { SinglePlayer, TwoPlayers, Handling, Exit };

private:
  Resolution resolution = Resolution::Small;
  std::pair<int, int> windowResolution = {InitialWidth, InitialHeight};
  Option selectedOption = Option::SinglePlayer;

  virtual void draw() const override;
  virtual void update() override;
  void resizeScreen();

public:
  Menu() = default;
  virtual bool shouldStopRunning() const final;
  Option getSelectedOption() const;
  Option runAndGetSelectedOption();
};

#endif
