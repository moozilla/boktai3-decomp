#include "global.h"
#define ActorPosition_TestPath sub_0817A8C8
struct ActorPathPosition {
    s16 x, y, z, pad;
};
struct ActorPathState {
    u8 unknown0[0x54];
    struct ActorPathPosition position;
    u8 unknown5C[0x22C];
    u8 pathContext[1];
};
s32 sub_0821F204(void *, struct ActorPathPosition *, struct ActorPathPosition *);
u32 sub_0821F2EC(void *, void *, void *);
s32 sub_0817A8C8(struct ActorPathState *p, struct ActorPathPosition *destination)
{
    if (sub_0821F204(p->pathContext, &p->position, destination) &&
        sub_0821F2EC(p->pathContext, &p->position, destination))
        return 1;
    return 0;
}
