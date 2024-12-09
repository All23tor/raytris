#ifndef MENU_H
#define MENU_H

#include <utility>

enum class Resolutions { SMALL, MEDIUM, BIG, FULLSCREEN };
constexpr int INITIAL_WIDTH = 640;
constexpr int INITIAL_HEIGHT = 360;

class Menu {
  Resolutions resolution = Resolutions::SMALL;
  std::pair<int, int> windowResolution;

  void draw() const;
  void update();
  void resizeScreen();

public:
  Menu();
  enum class ExitCode { Game, Exit };
  ExitCode run();
  const std::pair<int, int> getResolution(Resolutions resolution) const;
};

#endif
