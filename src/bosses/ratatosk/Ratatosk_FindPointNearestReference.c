#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

s32 Ratatosk_FindPointNearestReference(struct Ratatosk *self)
{
    s32 selected = -1;
    u32 minimum = -1;
    s32 i;

    for (i = 0; i < 9; i++) {
        if (self->orbPoints[i].anchorDistance <= minimum) {
            minimum = self->orbPoints[i].anchorDistance;
            selected = i;
        }
    }
    return selected;
}
