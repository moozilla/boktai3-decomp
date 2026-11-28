#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

s32 sub_081F339C(struct Ratatosk *self)
{
    s32 selected = -1;
    u32 maximum = 0;
    s32 i;

    for (i = 0; i < 4; i++) {
        if (self->targets[i].active == 1 && self->targets[i].anchorDistance >= maximum) {
            maximum = self->targets[i].anchorDistance;
            selected = i;
        }
    }
    return selected;
}
