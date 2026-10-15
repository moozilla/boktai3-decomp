#ifndef GUARD_REPEATED_EFFECT_H
#define GUARD_REPEATED_EFFECT_H
#include "game_state.h"
/* Provisional identity: eight script-created effects with repeated state callbacks. */
struct RepeatedEffectPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct RepeatedEffectSprite {
    u32 flags;
    u8 unknown4[3];
    u8 renderMode;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[10];
    struct RepeatedEffectPosition position;
    u8 unknown24[8];
};
struct RepeatedEffectCollider {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[0x2C];
    u32 responseFlags, contactFlags;
    u16 unknown3C, value;
    u8 unknown40[4];
    u16 cooldown;
    u8 unknown46[0xE];
};
struct RepeatedEffect {
    struct RepeatedEffectSprite sprite;
    u8 graphics[0x1C], body[0x48];
    struct RepeatedEffectCollider collider;
    u8 state, behaviorFlags, unknownE6, request;
    u16 tag, transitionHold;
    u8 bodyRegistered, flashing, constrainCell, constrained;
    u16 cell, unknownF2;
    s16 balance;
    u16 stateFrame;
    void (*update)(struct RepeatedEffect *);
};
struct RepeatedEffectManager {
    u8 task[0x18];
    u16 count, unknown1A;
    struct RepeatedEffect entries[8];
};
extern struct RepeatedEffectManager *gUnk_02000110;
void sub_0804F7D0(struct RepeatedEffect *, u8, void (*)(struct RepeatedEffect *));
void sub_0804F8CC(struct RepeatedEffect *);
void sub_0804F888(struct RepeatedEffect *);
void sub_0804F91C(struct RepeatedEffect *);
void sub_0804F9C4(struct RepeatedEffect *);
void sub_0804F970(struct RepeatedEffect *);
void sub_0804FB0C(struct RepeatedEffect *);
void sub_0804FB48(struct RepeatedEffect *);
#endif
