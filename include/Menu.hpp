#ifndef MENU_H
#define MENU_H

#include "Runnable.hpp"

class Menu : public Runnable {
public:
  enum class Option { SinglePlayer, TwoPlayers, Handling, Exit };

private:
  Option selectedOption = Option::SinglePlayer;
  virtual void draw() const override;
  virtual void update() override;

public:
  Menu() = default;
  virtual bool shouldStopRunning() const final;
  Option getSelectedOption() const;
  Option runAndGetSelectedOption();
};

#endif
