#include "global.h"
struct DriftEffect {
    u8 pad0[0x24];
    u32 flags;
    u8 pad28[0x40 - 0x28];
    u16 x;
    u8 pad42[2];
    u16 y;
    u8 pad46[0xD0 - 0x46];
    u16 timer;
    u8 entered;
    u8 state;
};
// confidence=low: moves 20 units along x (flag 4) or y for three frames, then switches to state 3.
void sub_08199BD8(struct DriftEffect *p)
{
    if (p->flags & 4)
        p->x += 20;
    else
        p->y += 20;
    p->timer++;
    if (p->timer > 2) {
        u8 state = 3;
        p->state = state;
        p->entered = 0;
        p->timer = 0;
    }
}
