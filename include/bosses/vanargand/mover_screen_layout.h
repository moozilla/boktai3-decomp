#ifndef GUARD_INCLUDE_BOSSES_VANARGAND_MOVER_SCREEN_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_VANARGAND_MOVER_SCREEN_LAYOUT_H

#include "global.h"

struct MoverScreen {
    s16 x;
    s16 y;
    s16 z;
    s16 unused; /* confidence=low */
} __attribute__((aligned(4)));

typedef char MoverScreenSizeCheck[sizeof(struct MoverScreen) == 0x8 ? 1 : -1];

#endif
