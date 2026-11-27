#ifndef GUARD_INCLUDE_BOSSES_VANARGAND_VANARGAND_PAIR_STATE_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_VANARGAND_VANARGAND_PAIR_STATE_LAYOUT_H

#include "global.h"

struct VanargandPairState {
    u8 pad0[0xae];
    u8 mode;
    u8 padAF[0x32];
    u8 gauge; /* confidence=low */
    u8 padE2[0x48a];
    void *pair;
    u8 pad570[0x50];
    s16 counter;
    s16 variant;
    u8 state;
    u8 cursor;
    u8 transition;
    u8 level; /* confidence=low */
};

typedef char VanargandPairStateSizeCheck[sizeof(struct VanargandPairState) == 0x5c8 ? 1 : -1];

#endif
