#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

s32 Ratatosk_FindPointNearestPlayer(struct Ratatosk *self)
{
    s32 selected = -1;
    u32 minimum = -1;
    s32 i;

    for (i = 0; i < 9; i++) {
        if (self->orbPoints[i].distance <= minimum) {
            minimum = self->orbPoints[i].distance;
            selected = i;
        }
    }
    return selected;
}
