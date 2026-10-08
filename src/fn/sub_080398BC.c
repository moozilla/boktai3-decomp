#include "global.h"
struct G { u8 pad[0x52]; s16 i; u8 pad2[0x34]; u16 t[1]; };
extern struct G *gUnk_02000710;
u32 sub_080398BC(void)
{
    struct G *g = gUnk_02000710;
    u32 r;
    switch ((s16)(((u16 *)((u8 *)g + g->i * 2))[0x44] - 6)) {
    case 0: r = 1; break;
    case 1: r = 4; break;
    case 2: r = 6; break;
    case 3:
    case 4:
    case 5: r = 3; break;
    default: r = 0; break;
    }
    return r;
}
