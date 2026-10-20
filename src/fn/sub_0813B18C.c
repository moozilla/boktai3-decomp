#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct DirectionActor {
    u8 pad00[6];
    u16 flags;
    u8 pad08[0x1c];
    s16 x;
    u16 pad26;
    s16 y;
    u8 pad2A[0x18];
    u8 direction;
};
s32 Math_Atan2(s32, s32);
s32 ActorDirection_ComputeRelativeCardinal(struct DirectionActor *actor,
                                           struct DirectionActor *target)
{
    s32 angle;
    if (actor->flags & 0x100) {
        angle = actor->direction + 0xe0;
        asm("" ::: "memory");
    } else {
        s32 dx = actor->x - target->x;
        s32 dy = actor->y - target->y;
        if (dx == 0 && dy == 0)
            angle = target->direction + 0xe0;
        else
            angle = Math_Atan2(dx, dy) + 0x60;
    }
    angle &= 0xff;
    return (angle >> 6) * 2 + 1;
}
