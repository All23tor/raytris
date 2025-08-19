#ifndef CONTROLLER_H
#define CONTROLLER_H

struct Controller {
  using Input = bool (*)();
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
};

#endif
