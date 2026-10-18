#include "global.h"
struct ActorResourceState {
    u8 unknown0[0x274];
    u16 limits[2];
    u8 unknown278[0x272];
    u8 selection;
};
s32 ActorResource_IsBelowSelectedLimit(struct ActorResourceState *p, s32 value)
{
    if (p->selection == 0) {
        if (value < p->limits[0])
            return 1;
    } else if (p->selection == 1) {
        if (value < p->limits[1])
            return 1;
    }
    return 0;
}
