#include "tether_actor.h"
void TetherActor_RemoveSegments(struct TetherActor *actor)
{
    struct TetherSegment *segment = actor->segments;
    s32 remaining = 6;
    do {
        Effect_Remove(segment);
        --remaining;
        ++segment;
    } while (remaining >= 0);
}
