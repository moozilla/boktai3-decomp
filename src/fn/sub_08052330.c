#include "global.h"
struct G {
    u8 pad[0x30];
    s16 x;
    s16 padx;
    s16 y;
};
struct O {
    u8 pad[0x98];
    s16 x;
    s16 padx;
    s16 y;
};
extern struct G *gUnk_02000710;
s32 sub_08052330(struct O *o)
{
    struct G *g = gUnk_02000710;
    s32 dx = g->x - o->x;
    s32 dy;
    if (dx < 0)
        dx = -dx;
    dy = g->y - o->y;
    if (dy < 0)
        dy = -dy;
    if (dx > 0x60 || dy > 0x60)
        return 0;
    if (dx * dx + dy * dy > 0x2400)
        return 0;
    return 1;
}
