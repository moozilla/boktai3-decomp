#define WoodenCrate_CheckTrigger  sub_080FFDD4
#define WoodenCrate_TestFlag WoodenCrate_HasFlags
#define WoodenCrate_StartSequence sub_080FFAF8
#include "global.h"
struct WoodenCrate {
    u8 pad0[0x66];
    u16 flags;
    u8 pad68[0xA4 - 0x68];
    u16 timer;
    u8 padA6[0xCE - 0xA6];
    u16 triggerFlags;
    u16 value;
    u16 threshold;
};
u8 WoodenCrate_HasFlags(struct WoodenCrate *, s32);
void sub_080FFAF8(struct WoodenCrate *);
void sub_080FFDD4(struct WoodenCrate *p)
{
    if (WoodenCrate_HasFlags(p, 4)) {
        if (p->triggerFlags & 1) {
            if (WoodenCrate_HasFlags(p, 1))
                return;
        } else {
            if (WoodenCrate_HasFlags(p, 2))
                return;
        }
        {
            p->flags |= 4;
            if (!WoodenCrate_HasFlags(p, 8) && p->value <= p->threshold) {
                p->timer = 24;
                sub_080FFAF8(p);
            }
        }
    }
}
