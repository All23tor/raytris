#ifndef MENU_H
#define MENU_H

#include <utility>

enum class Resolutions { SMALL, MEDIUM, BIG, FULLSCREEN };
std::pair<int, int> getResolution(Resolutions resolution);

class Menu {
  Resolutions resolution = Resolutions::SMALL;
  int screenHeight;
  int screenWidth;
  unsigned char DAS;

  void draw() const;
  void update();
  void toggleFullscreen();

public:
  void run();
};

#endif
