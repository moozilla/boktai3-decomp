// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 61.2%
// confidence=low: walks toward the target while updating facing and tagged animation.
// Remaining differences: distance-result allocation and retained movement pointers.
#include "state_actor.h"
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
void TaggedSceneNpc_PlayTaggedAnimation(void *, s32, s32, s32, s32);
static inline s32 DistanceSquared(struct StateActorPos *a, struct StateActorPos *b)
{
    s32 x = a->x - b->x;
    s32 z = a->z - b->z;
    return x * x + z * z;
}
static inline s32 ScaleOffset(s32 scale, s32 value)
{
    value *= scale;
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
static inline void facing(struct StateActor *npc, short angle)
{
    npc->angle = angle;
    {
        s32 t = (s32)(angle + 0x70) >> 5;
        u32 m = 7;
        m &= t;
        npc->facing8 = m;
    }
    {
        s32 t = (s32)(angle + 0x60) >> 6;
        u32 m = 3;
        m &= t;
        npc->facing4 = m;
    }
}
void sub_081D1A8C(void *context, struct StateActor *p)
{
    struct StateActorPos *position, *target;
    if (p->enter) {
        p->enter = 0;
        p->done = 0;
        TaggedSceneNpc_PlayTaggedAnimation(p, 1, 1, 0, 4);
    }
    position = &p->position;
    target = &p->target;
    if (DistanceSquared(target, position) <= 100) {
        p->position = p->target;
        p->done = 1;
    } else {
        u8 *angle = &p->angle;
        struct StateActorPos *velocity = (void *)((u8 *)p + 0x70);
        register s32 heading asm("r6") = *angle;
        register s32 speed asm("r8") = 5;
        asm("" : "+r"(speed));
        velocity->x = ScaleOffset(speed, gUnk_086149C4[(heading + 64) & 255]);
        velocity->y = 0;
        velocity->z = ScaleOffset(speed, gUnk_086149C4[heading]);
        p->targetAngle = Math_Atan2(target->x - position->x, target->z - position->z);
        facing(p, p->targetAngle);
        TaggedSceneNpc_PlayTaggedAnimation(p, 1, 1, 0, 5);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081D1A8C);
#endif
