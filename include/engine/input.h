#ifndef GUARD_INPUT_H
#define GUARD_INPUT_H

#include "global.h"

/* RAM objects; each row of symbols/ram.csv with header include/input.h. */
/* 0x03005260 */
extern u16 gPadState
    []; /* input state: two slots of {held, pressed, released, pad}; updated by sub_08219B24 */

#endif
