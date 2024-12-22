#include "Controller.hpp"

Controller::Controller(
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
): 
  checkRestartInput(std::move(restartInput)),
  checkSwapInput(std::move(swapInput)),
  checkLeftInput(std::move(leftInput)),
  checkRightInput(std::move(rightInput)),
  checkLeftDasInput(std::move(leftDasInput)),
  checkRightDasInput(std::move(rightDasInput)),
  checkClockwiseInput(std::move(clockwiseInput)),
  checkCounterClockwiseInput(std::move(counterClockwiseInput)),
  checkOneEightyInput(std::move(oneEightyInput)),
  checkHardDropInput(std::move(hardDropInput)),
  checkSoftDropInput(std::move(softDropInput)),
  checkUndoInput(std::move(undoInput)),
  checkPauseInput(std::move(pauseInput)),
  checkQuitInput(std::move(quitInput))
{}