#ifndef GUARD_INCLUDE_NPC_KURO_KURO_COMMAND_ACTOR_LAYOUT_H
#define GUARD_INCLUDE_NPC_KURO_KURO_COMMAND_ACTOR_LAYOUT_H

#include "global.h"

struct KuroCommandActor {
    u8 f[4]; /* confidence=low */
    u8 b4;
    u8 b5;
    u8 f6;
    u8 b7;
    u8 b8;
    u8 g[0x14 - 9]; /* confidence=low */
    u32 c14;
    u8 g2[0x168 - 0x18];
    u8 d[1];
    u8 pad169[0x3];
};

typedef char KuroCommandActorSizeCheck[sizeof(struct KuroCommandActor) == 0x16c ? 1 : -1];

#endif
