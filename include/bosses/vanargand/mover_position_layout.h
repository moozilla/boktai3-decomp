#ifndef GUARD_INCLUDE_BOSSES_VANARGAND_MOVER_POSITION_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_VANARGAND_MOVER_POSITION_LAYOUT_H

#include "global.h"

struct MoverPosition {
    s32 x;
    s32 y;
    s32 z;
};

typedef char MoverPositionSizeCheck[sizeof(struct MoverPosition) == 0xc ? 1 : -1];

#endif
