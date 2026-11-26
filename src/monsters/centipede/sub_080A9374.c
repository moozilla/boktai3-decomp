#include "monsters/centipede/centipede_body.h"

void Sprite_BlinkUntilExpired(void *, void *, u32, u32);

static inline u8 TestMask(u8 *flags, u32 mask)
{
    if (*flags & mask)
        return TRUE;
    return FALSE;
}

void CentipedeBody_UpdateSegmentMotion(struct RoomActor *actor, void *arg)
{
    struct TentacleBody *g = actor->context;
    struct TentacleAnimationSegment *e = g->segments;
    s32 i;
    for (i = 0; i < g->segmentCount; i++) {
        if (!TestMask(&e[i].animation.status, 1))
            Sprite_BlinkUntilExpired(&e[i], arg, 10, 30);
    }
}
