#include "objects/warp_circle/warp_circle.h"
void Trigger_ConsumePending(void *, u32);
void WarpCircle_RetractToIdle(struct WarpCircle *o)
{
    o->sprite.flags &= ~1;
    if (o->sprite.position.y < o->world.y + 22)
        o->sprite.position.y += 2;
    if (o->height != 0) {
        o->height--;
        WarpCircle_UpdateSegments(o);
    } else {
        o->shadow.flags &= ~1;
        WarpCircle_SetUpdate(o, WarpCircle_TickIdle);
        Trigger_ConsumePending(&o->trigger, 1);
    }
}
