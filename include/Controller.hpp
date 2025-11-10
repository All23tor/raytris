#ifndef CONTROLLER_H
#define CONTROLLER_H

struct Controller {
  using Input = bool (*)();
  Input restart;
  Input swap;
  Input left;
  Input right;
  Input left_das;
  Input right_das;
  Input clockwise;
  Input counter_clockwise;
  Input one_eighty;
  Input chech_hard_drop;
  Input soft_drop;
  Input undo;
  Input pause;
  Input quit;
};

#endif
