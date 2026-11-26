#include "objects/warp_circle/warp_circle.h"
/* WarpCircle_RetractSegments: Retracts stacked segments then selects disappearance or idle; preserves the existing provisional source alias */
void WarpCircle_RetractSegments(struct WarpCircle *actor)
{
    actor->sprite.flags &= ~1;
    if (actor->sprite.position.y < actor->world.y + 22)
        actor->sprite.position.y += 2;
    if (actor->height) {
        actor->height--;
        WarpCircle_UpdateSegments(actor);
    } else {
        actor->shadow.flags &= ~1;
        if (actor->removeAfterUse)
            WarpCircle_SetUpdate(actor, WarpCircle_Disappear);
        else
            WarpCircle_SetUpdate(actor, WarpCircle_TickIdle);
    }
}
