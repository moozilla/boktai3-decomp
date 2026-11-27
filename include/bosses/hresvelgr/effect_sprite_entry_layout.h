#ifndef GUARD_INCLUDE_BOSSES_HRESVELGR_EFFECT_SPRITE_ENTRY_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_HRESVELGR_EFFECT_SPRITE_ENTRY_LAYOUT_H

#include "global.h"

struct EffectSpriteEntry {
    u32 flags;
    u8 pad4[0x48];
    u32 owner;
    u16 state;
    u8 pad52[0x6];
    u8 frame;
    u8 mode;
    u8 pad5A[0x2];
};

typedef char EffectSpriteEntrySizeCheck[sizeof(struct EffectSpriteEntry) == 0x5c ? 1 : -1];

#endif
