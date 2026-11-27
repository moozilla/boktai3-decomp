#include "monsters/centipede/centipede_body.h"
inline static u8 TestMask(struct TentacleAnimationSegment *e, char mask)
{
    if (e->animation.status & mask)
        return 1;
    return 0;
}
void Centipede_AssignSegmentPhases(void *context)
{
    struct TentacleBody *g = context;
    s32 i;
    s32 k = 0;
    for (i = 0; i < g->segmentCount; i++) {
        if (!TestMask(&g->segments[i], 5)) {
            g->segments[i].animation.sequence.index = k;
            k++;
            if (k > 3)
                k = 0;
        }
    }
}
