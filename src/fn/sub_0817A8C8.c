#include "global.h"
struct ActorPathPosition {
    s16 x, y, z, pad;
};
struct ActorPathState {
    u8 unknown0[0x54];
    struct ActorPathPosition position;
    u8 unknown5C[0x22C];
    u8 pathContext[1];
};
s32 Nav_IsPositionInMesh(void *, struct ActorPathPosition *, struct ActorPathPosition *);
u32 NavMesh_FindRoute(void *, void *, void *);
s32 ActorPosition_TestPath(struct ActorPathState *p, struct ActorPathPosition *destination)
{
    if (Nav_IsPositionInMesh(p->pathContext, &p->position, destination) &&
        NavMesh_FindRoute(p->pathContext, &p->position, destination))
        return 1;
    return 0;
}
