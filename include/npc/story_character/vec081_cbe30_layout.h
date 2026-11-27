#ifndef GUARD_INCLUDE_NPC_STORY_CHARACTER_VEC081_CBE30_LAYOUT_H
#define GUARD_INCLUDE_NPC_STORY_CHARACTER_VEC081_CBE30_LAYOUT_H

#include "global.h"

struct Vec081CBE30 {
    s16 x;
    s16 y;
    s16 z;
    s16 pad; /* confidence=low */
} __attribute__((aligned(4)));

typedef char Vec081CBE30SizeCheck[sizeof(struct Vec081CBE30) == 0x8 ? 1 : -1];

#endif
