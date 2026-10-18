// MATCHING_CONCESSION: register-pinning
#include "global.h"
typedef struct ActorAnimation {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[5];
    u8 finished, enter;
    u8 changed;
    u8 padB2;
    u8 locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x37];
    u8 (*tryUpdate)(void *);
    u8 (*reset)(void *);
    u8 pad10C[8];
    s8 savedAngle;
    u8 pad115[0x22e];
    u8 variant;
    s16 scale, minScale, maxScale, scaleStep;
    u8 pad34C[0x60];
    u32 mode;
} ActorAnimation;

static inline void setFacing(ActorAnimation *p, int angle)
{
    u32 value;
    register int shifted asm("r0");
    p->angle = angle;
    shifted = p->angle + 0x60;
    asm("" : "+r"(shifted));
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    asm("" : "+r"(shifted));
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}
void GridAttackActor_SetAnimation(void *, int, int, int);
void GridAttackActor_ResetAttackAnimationHistory(void *);
int GridAttackActor_SelectAttackAnimation(void *);
void GridAttackActor_SpawnGridPattern(void *);
void Sound_PlaySongChecked(int);
void GridAttackActor_UpdateGridAttack(ActorAnimation *p, int tick)
{
    if (p->changed) {
        p->changed = 0;
        setFacing(p, p->requestedAngle);
        GridAttackActor_ResetAttackAnimationHistory(p);
        GridAttackActor_SetAnimation(p, GridAttackActor_SelectAttackAnimation(p), 1, 8);
        Sound_PlaySongChecked(1355);
    }
    if (p->tryUpdate(p))
        return;
    {
        u32 mask = 64;
        p->flags |= mask;
    }
    if (p->reset(p))
        return;
    if (tick == 8)
        GridAttackActor_SpawnGridPattern(p);
    if (p->padB2) {
        GridAttackActor_SetAnimation(p, GridAttackActor_SelectAttackAnimation(p), 1, 8);
        if (tick > 31)
            p->finished = 1;
    }
}
