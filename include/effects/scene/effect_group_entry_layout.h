#ifndef GUARD_INCLUDE_EFFECTS_SCENE_ACTOR_EFFECTS_EFFECT_GROUP_ENTRY_LAYOUT_H
#define GUARD_INCLUDE_EFFECTS_SCENE_ACTOR_EFFECTS_EFFECT_GROUP_ENTRY_LAYOUT_H

#include "global.h"

struct EffectGroupEntry {
    u8 state;
    u8 fadeTimer;
    u16 frame;
    u8 pad4[0x4];
    u16 x; /* confidence=low */
    u16 y; /* confidence=low */
    u16 z; /* confidence=low */
    u8 padE[0x2];
    u32 flags; /* confidence=low */
    u8 pad14[0x24];
    u8 anim[0x10]; /* confidence=low */
};

typedef char EffectGroupEntrySizeCheck[sizeof(struct EffectGroupEntry) == 0x48 ? 1 : -1];

#endif
