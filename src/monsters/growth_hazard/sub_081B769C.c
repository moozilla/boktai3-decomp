#include "global.h"
void TerrainModifiers_RegisterCell(void *, u32, u32, u32, u32, u32);
static inline int trunc8(int x)
{
    return x >= 0 ? x >> 8 : -(-x >> 8);
}
void AnimatedMarker_RegisterOccupiedCell(u8 *p)
{
    u32 f = *(u32 *)p & 4;
    u32 kind = 1;
    if (f)
        kind = 2;
    TerrainModifiers_RegisterCell(p + 0x104, *(u16 *)(p + 0xf2), kind,
                                  trunc8(*(s16 *)(p + 0xe6)) + 1, 0xff, 0);
    p[0x103] = 1;
}
