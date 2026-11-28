#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

s32 sub_081F336C(struct Ratatosk *self)
{
    s32 selected = -1;
    u32 maximum = 0;
    s32 i;

    for (i = 0; i < 4; i++) {
        if (self->targets[i].active == 1 && self->targets[i].distance >= maximum) {
            maximum = self->targets[i].distance;
            selected = i;
        }
    }
    return selected;
}
