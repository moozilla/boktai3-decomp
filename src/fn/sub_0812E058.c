#define ActorFacing_TurnTowardPoint sub_0812E058
#include "global.h"

struct FacingActor {
    u8 pad0[9];
    u8 facing;
    u8 padA[2];
    u16 x;
    u8 padE[2];
    u16 y;
};
s32 Math_Atan2(s32, s32);

static inline s32 FacingDelta(s16 target, u16 current)
{
    return target - current;
}

void sub_0812E058(struct FacingActor *actor, s32 x, s32 y)
{
    s32 dx = FacingDelta(x, actor->x);
    s32 dy = FacingDelta(y, actor->y);
    actor->facing = Math_Atan2((s16)dx, (s16)dy);
}
