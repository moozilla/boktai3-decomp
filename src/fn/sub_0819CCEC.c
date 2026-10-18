#define OffsetActor_StepTowardTarget sub_0819CCEC
#include "global.h"
struct OffsetActorVector {
    s16 x, y, z, pad;
};
struct OffsetActor {
    u8 pad0[0x54];
    struct OffsetActorVector position, motion;
    u8 pad64[0x39C - 0x64];
    struct OffsetActorVector target;
};
s32 Math_StepTowardTarget(s32, s32, s32);
static inline void Difference(struct OffsetActorVector *out, struct OffsetActorVector *a,
                              struct OffsetActorVector *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
void sub_0819CCEC(struct OffsetActor *p)
{
    struct OffsetActorVector v;
    struct OffsetActorVector *current = &p->position, *target = &p->target;
    s32 x = Math_StepTowardTarget(current->x, target->x, 12);
    s32 y = Math_StepTowardTarget(current->y, target->y, 12);
    s32 z = Math_StepTowardTarget(current->z, target->z, 12);
    struct OffsetActorVector *out = &v;
    out->x = x;
    out->y = y;
    out->z = z;
    Difference(&p->motion, &v, &p->position);
}
