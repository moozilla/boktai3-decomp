#ifndef GUARD_INPUT_H
#define GUARD_INPUT_H

#include "global.h"

/* RAM objects; each row of symbols/ram.csv with header include/input.h. */
/* Input_UpdatePadStates updates two input slots of {held, pressed, released, pad}. */
/* 0x03005260 */
extern u16 gPadState[];

#endif
