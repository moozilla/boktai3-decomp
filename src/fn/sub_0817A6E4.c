#include "global.h"
struct ActorPositionPoint {
    s16 x, y, z, pad;
};
struct ActorOffsetPointState {
    u8 unknown0[0x54];
    struct ActorPositionPoint position;
    u8 unknown5C[0x59C];
    struct ActorPositionPoint offsetPoint;
};
void ActorPosition_SetOffsetPoint(struct ActorOffsetPointState *p)
{
    p->offsetPoint = p->position;
    p->offsetPoint.x -= 20;
    p->offsetPoint.y += 330;
    p->offsetPoint.z += 60;
}
