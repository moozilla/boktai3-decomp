#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

s32 Ratatosk_FindActiveTargetNearestReference(struct Ratatosk *self)
{
    s32 selected = -1;
    u32 minimum = -1;
    s32 i;

    for (i = 0; i < 4; i++) {
        if (self->targets[i].active == 1 && self->targets[i].anchorDistance <= minimum) {
            minimum = self->targets[i].anchorDistance;
            selected = i;
        }
    }
    return selected;
}
