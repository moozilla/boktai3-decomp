#include "tether_actor.h"
#define TetherActor_RemoveSegments sub_08048898
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
