#include "global.h"

u32 sub_08002AC0(u8 *p, u32 a, u32 b);
void Palette_ScaleRange(u8 *x, u32 y, u32 z, u32 w, u32 q);

void sub_08002E00(u8 *p, u8 *q)
{
    u32 r = sub_08002AC0(p, 0, 2);
    Palette_ScaleRange(q + 0xC, r, 0x32, 0, 0xD0);
}
