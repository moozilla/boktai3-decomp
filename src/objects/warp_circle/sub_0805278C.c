#include "objects/warp_circle/warp_circle.h"
void WarpCircle_LowerLift(struct WarpCircle *actor)
{
    u8 *lift = &actor->lift;
    u32 value = *lift;
    if (value > 1)
        value -= 2;
    else
        value = 0;
    *lift = value;
    actor->sprite.position.y = (u16)actor->world.y + actor->lift;
    if (!actor->lift) {
        actor->sprite.flags &= ~0x200;
        actor->sprite.renderMode = 3;
        actor->shadow.flags |= 1;
        WarpCircle_SetUpdate(actor, WarpCircle_ExtendSegments);
    }
}
