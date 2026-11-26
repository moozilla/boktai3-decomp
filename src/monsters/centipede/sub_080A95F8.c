#include "monsters/centipede/centipede_body.h"
#include "global.h"

static inline u8 TestMask(u8 flags, u32 mask)
{
    if (flags & mask)
        return TRUE;
    return FALSE;
}

void SpritePool_FlagEntries(struct TentacleBody *g)
{
    s32 i = 0;
    s16 *countPtr = &g->segmentCount;
    if (i < *countPtr) {
        u32 mask = 1;
        s16 *savedCountPtr = countPtr;
        struct TentacleAnimationSegment *e = g->segments;
        u8 *flagsPtr = &e->animation.status;
        u32 *fieldPtr = &e->flags;
        do {
            if (TestMask(*flagsPtr, mask))
                *fieldPtr = mask;
            flagsPtr += sizeof(*e);
            fieldPtr = (u32 *)((u8 *)fieldPtr + sizeof(*e));
            i++;
        } while (i < *savedCountPtr);
    }
}
