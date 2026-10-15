#ifndef GUARD_AREA_EFFECT_LINK_H
#define GUARD_AREA_EFFECT_LINK_H
#include "area_effect.h"
/* Observed link-session cache, identity of byte values remains provisional. */
struct AreaEffectLinkState {
    u8 unknown0[0x18];
    s16 localPlayer;
    u8 unknown1A[0x42];
    u8 playerValues[4];
    u8 unknown60[2];
    u16 value;
};
extern struct AreaEffectLinkState *gUnk_020004B4;
extern struct AreaEffectPlayer *gUnk_02000580[4];
#endif
