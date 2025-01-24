#ifndef MENU_H
#define MENU_H

class Menu {
public:
  enum class Option { SinglePlayer, TwoPlayers, Settings, Exit };

private:
  Option selectedOption = Option::SinglePlayer;

public:
  void draw() const;
  void update();
  bool shouldStopRunning() const;
  Option getSelectedOption() const;
};

#endif
