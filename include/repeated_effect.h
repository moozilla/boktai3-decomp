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
extern struct RepeatedEffectManager *gRepeatedEffectManager;
void RepeatedEffect_SetState(struct RepeatedEffect *, u8, void (*)(struct RepeatedEffect *));
void RepeatedEffect_TickLowerTransition(struct RepeatedEffect *);
void RepeatedEffect_TickLowerState(struct RepeatedEffect *);
void RepeatedEffect_TickLowerRecovery(struct RepeatedEffect *);
void RepeatedEffect_TickUpperTransition(struct RepeatedEffect *);
void RepeatedEffect_TickUpperState(struct RepeatedEffect *);
void RepeatedEffect_TickUpperRecovery(struct RepeatedEffect *);
void RepeatedEffect_TickNeutral(struct RepeatedEffect *);
void RepeatedEffect_InitParameters(struct RepeatedEffect *, u32, struct RepeatedEffectPosition *,
                                   u32, u32, u32);
void RepeatedEffect_InitSprite(struct RepeatedEffect *, struct RepeatedEffectPosition *, u32);
void RepeatedEffect_InitCollider(struct RepeatedEffect *);
void RepeatedEffect_InitState(struct RepeatedEffect *);
void RepeatedEffect_InitEntry(struct RepeatedEffectManager *, u32, struct RepeatedEffectPosition *,
                              u32, u32, u32, u32);
void RepeatedEffect_TickEntry(struct RepeatedEffect *);
s32 RepeatedEffect_TickManager(struct RepeatedEffectManager *);
s32 RepeatedEffect_DestroyManager(struct RepeatedEffectManager *);
s32 RepeatedEffect_InitManager(struct RepeatedEffectManager *);
struct RepeatedEffectManager *RepeatedEffect_CreateManager(void);
#endif
