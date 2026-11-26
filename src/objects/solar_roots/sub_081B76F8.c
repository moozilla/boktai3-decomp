#include "global.h"

struct O {
    u8 f0[0xe6];
    s16 he6;
    u8 g[0xb];
    u8 gg;
    u16 hf2;
    u8 g2[0x11];
    u8 sub[1];
};
void TerrainModifiers_RegisterCell(void *, u16, s32, s32, s32, s32);

void GrowthHazard_RegisterCellEffect(u8 *p)
{
    s32 v = *(s16 *)(p + 0xe6);
    s32 t;
    if (v >= 0)
        t = v >> 8;
    else
        t = -((-v) >> 8);
    TerrainModifiers_RegisterCell(p + 0x104, *(u16 *)(p + 0xf2), 0, t, 0xff, 0x801);
    *(u8 *)(p + 0x103) = 1;
}
