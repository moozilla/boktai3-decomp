#include "global.h"
extern const u32 gUnk_08605E48[8];
// MATCHING_CONCESSION: register-pinning
// confidence=low: maps the movement-curve progress to its capped eight-entry rate table.
u32 Lun_GetRouteProgressRate(u8 *p)
{
    u8 *q = *(u8 **)(p + 0x3D0);
    u32 phase = *(u16 *)(q + 0x3E0) >> 7;
    s32 i;
    asm("" : : "r"(phase));
    i = phase;
    if (i > 7)
        i = 7;
    return gUnk_08605E48[i];
}
