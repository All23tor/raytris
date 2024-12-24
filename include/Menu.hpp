#ifndef MENU_H
#define MENU_H

#include <utility>

enum class Resolution { Small, Medium, Big, FullScreen };

class Menu {
public:
  static constexpr int InitialWidth = 640;
  static constexpr int InitialHeight = 360;
  enum class Option { SinglePlayer, TwoPlayers, Exit };

private:
  Resolution resolution = Resolution::Small;
  std::pair<int, int> windowResolution = {InitialWidth, InitialHeight};
  Option selectedOption = Option::SinglePlayer;

  void draw() const;
  void update();
  void resizeScreen();

public:
  Menu() = default;
  Option run();
  const std::pair<int, int> getResolution(Resolution resolution) const;
};

#endif
