#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <functional>

using Input = std::function<bool()>;

struct Controller {
  const Input checkRestartInput;
  const Input checkSwapInput;
  const Input checkLeftInput;
  const Input checkRightInput;
  const Input checkLeftDasInput;
  const Input checkRightDasInput;
  const Input checkClockwiseInput;
  const Input checkCounterClockwiseInput;
  const Input checkOneEightyInput;
  const Input checkHardDropInput;
  const Input checkSoftDropInput;
  const Input checkUndoInput;
  const Input checkPauseInput;
  const Input checkQuitInput;

  Controller() = delete;
  Controller(
    Input restartInput,
    Input swapInput,
    Input leftInput,
    Input rightInput,
    Input leftDasInput,
    Input rightDasInput,
    Input clockwiseInput,
    Input counterClockwiseInput,
    Input oneEightyInput,
    Input hardDropInput,
    Input softDropInput,
    Input undoInput,
    Input pauseInput,
    Input quitInput
  );
};

#endif