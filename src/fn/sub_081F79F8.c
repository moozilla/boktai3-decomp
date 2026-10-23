#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Actor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[2];
    struct Vec position, velocity;
    u8 pad64[0x2c];
    u32 timer;
    s16 health;
    u8 pad96[4];
    u8 damageEvent;
    u8 pad9B;
    u32 flags;
    u8 padA0[2];
    u8 mode, padA3;
    u32 modeTime;
    u8 facing, facing8, padAA, animationId, state, nextState, active, finished, padB0, changed,
        padB2, locked;
    u8 padB4[0x10];
    u32 stateCounter;
    u8 padC8[4];
    u8 requestedAngle;
    u8 padCD[0x33];
    void (*update)(void);
    void *callback;
    u8 pad108[0xc];
    s8 savedAngle;
    u8 pad115[0x67];
    u32 phase;
    u8 pad180[0x1ac];
    u8 animation[0x20];
    u8 animationData[0x20];
    struct Vec anchor;
    u8 pad374[0x58];
    struct Vec home;
    u8 pad3D4[0xa78];
    s16 selectedPoint, selectedTarget;
    u16 padE50;
    u16 scaleX, scaleZ, padE56, scaleStep;
    u8 padE5A[0x14];
    u16 motion;
    u8 motionFlag, padE71, targetAngle, phaseTimer;
    s16 hitTimer, terrainTimer;
};
void Ratatosk_UpdateDefeatSequence(struct Actor *, s32);
static inline void SetState(struct Actor *p, void (*update)(void), u8 state)
{
    p->animationId = state;
    p->update = update;
    p->timer = 0;
    p->changed = 1;
    p->mode = 0;
    p->modeTime = 0;
}
static inline u32 TestFlag(u32 mask, u32 *flags)
{
    return *flags & mask;
}
#define Ratatosk_ProcessDefeatState sub_081F79F8
s32 Ratatosk_ProcessDefeatState(struct Actor *p)
{
    if (p->health <= 0 && p->damageEvent) {
        u32 mask = 2;
        p->flags |= mask;
        SetState(p, (void (*)(void))Ratatosk_UpdateDefeatSequence, 15);
    }
    p->damageEvent = 0;
    if (TestFlag(2, &p->flags)) {
        u32 one = 1;
        p->state = one;
        p->nextState = 0;
        p->active = one;
        p->stateCounter = 0;
        p->padB0 = 1;
    } else if (p->flags & 1) {
        p->state = 1;
        p->nextState = 0;
        p->active = 0;
        p->stateCounter = 0;
        p->padB0 = 1;
    } else {
        return 0;
    }
    return 1;
}
