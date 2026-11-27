#ifndef GUARD_INCLUDE_EFFECTS_SCENE_ACTOR_EFFECTS_EFFECT_GROUP_LAYOUT_H
#define GUARD_INCLUDE_EFFECTS_SCENE_ACTOR_EFFECTS_EFFECT_GROUP_LAYOUT_H

#include "global.h"

struct Vec50;

struct EffectGroupEffectPart {
    u8 state;
    u8 pad1; /* confidence=low */
    u16 timer;
    u8 pad4[0x4];
    u16 x;
    u16 y;
    u16 z;
    u8 padE[0x1a];
    u16 screenX;
    u16 screenY;
    u16 screenZ;
    u8 pad2E[0x1a];
} __attribute__((aligned(4)));

typedef char EffectGroupEffectPartSizeCheck[sizeof(struct EffectGroupEffectPart) == 0x48 ? 1 : -1];

struct EffectGroup {
    u8 pad0[0x8];
    struct Vec50 *position;
    struct EffectGroupEffectPart parts[4];
};

typedef char EffectGroupSizeCheck[sizeof(struct EffectGroup) == 0x12c ? 1 : -1];

#endif
