#ifndef GUARD_INCLUDE_NPC_STORY_CHARACTER_NPC_TAG_LAYOUT_H
#define GUARD_INCLUDE_NPC_STORY_CHARACTER_NPC_TAG_LAYOUT_H

#include "global.h"

struct NpcTag {
    u8 pad0[0x2];
    s16 height;
} __attribute__((aligned(4)));

typedef char NpcTagSizeCheck[sizeof(struct NpcTag) == 0x4 ? 1 : -1];

#endif
