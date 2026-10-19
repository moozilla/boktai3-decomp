#include "global.h"

#ifdef NONMATCHING
// fuzzy: 93.9%
// foundry draft (worker best 91.7%); what differs is mostly register allocation and instruction order
#define Actor_InitLaunchVelocity sub_080DDD28

s32 Div(s32, s32);

struct ActorPosition {
    s16 x;
    s16 y;
    s16 z;
};

struct TargetEntity {
    u8 unk0[0xc];
    struct ActorPosition pos;
};

struct Actor {
    u8 unk0[0x5c];
    struct ActorPosition pos;
    u8 unk62[0xdc - 0x64];
    struct TargetEntity *target;
    u8 unke0[0x2ec - 0xe0];
    struct ActorPosition vel;
};

void Actor_InitLaunchVelocity(struct Actor *actor)
{
    s16 diff[3];
    s16 *pdiff = diff;
    struct ActorPosition *apos = &actor->pos;
    struct TargetEntity *target = actor->target;
    struct ActorPosition *tpos = &target->pos;

    pdiff[0] = tpos->x - apos->x;
    pdiff[1] = tpos->y - apos->y;
    pdiff[2] = tpos->z - apos->z;

    actor->vel.x = Div(pdiff[0], 0x1c);
    actor->vel.y = 0x20;
    actor->vel.z = Div(pdiff[2], 0x1c);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080DDD28);
#endif
