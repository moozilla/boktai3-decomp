#include "global.h"
#define ActorPosition_SetOffsetPoint sub_0817A6E4
struct ActorPositionPoint {
    s16 x, y, z, pad;
};
struct ActorOffsetPointState {
    u8 unknown0[0x54];
    struct ActorPositionPoint position;
    u8 unknown5C[0x59C];
    struct ActorPositionPoint offsetPoint;
};
void sub_0817A6E4(struct ActorOffsetPointState *p)
{
    p->offsetPoint = p->position;
    p->offsetPoint.x -= 20;
    p->offsetPoint.y += 330;
    p->offsetPoint.z += 60;
}
