#ifndef GUARD_INCLUDE_BOSSES_VANARGAND_VANARGAND_PART_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_VANARGAND_VANARGAND_PART_LAYOUT_H

#include "global.h"

struct VanargandPart {
    u8 pad0[0x10];
    u16 state;
    u8 pad12[0x2];
} __attribute__((aligned(4)));

typedef char VanargandPartSizeCheck[sizeof(struct VanargandPart) == 0x14 ? 1 : -1];

#endif
