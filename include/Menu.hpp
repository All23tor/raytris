#ifndef MENU_H
#define MENU_H

#include <utility>

enum class Resolutions { SMALL, MEDIUM, BIG, FULLSCREEN };
constexpr int INITIAL_WIDTH = 640;
constexpr int INITIAL_HEIGHT = 480;

class Menu {
  Resolutions resolution = Resolutions::SMALL;
  const int fullScreenWidth;
  const int fullScreenHeight;
  int windowHeight;
  int windowWidth;
  unsigned char DAS;

  void draw() const;
  void update();
  void resizeScreen();

public:
  Menu();
  void run();
  const std::pair<int, int> getResolution(Resolutions resolution) const;
};

#endif
