#include "objects/warp_circle/warp_circle.h"
void WarpCircle_SetUpdate(struct WarpCircle *actor, void (*update)(struct WarpCircle *))
{
    actor->update = update;
    actor->frame = 0;
}
