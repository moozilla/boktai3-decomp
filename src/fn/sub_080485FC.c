#include "tether_actor.h"
void TetherActor_InitializeSegments(struct TetherActor *actor)
{
    struct TetherSegment *segment = actor->segments;
    s32 offset = -4;
    s32 remaining = 6;
    do {
        void *resource = AnimData_FindEntry(0x1C1C);
        sub_08217DE0(segment, resource, 0);
        sub_08217EC4(segment, offset, offset);
        segment->priority = 255;
        segment->flags |= 1;
        --remaining;
        ++segment;
    } while (remaining >= 0);
}
