#include "global.h"
extern u8 *gTerrainMap;
u8 *sub_0821E654(u8 *p)
{
    s32 next;
    u8 *table;
    u8 *base;
    next = p[2];
    next++;
    table = *(u8 **)(p + 4);
    if (next >= table[0])
        next = 0;
    base = *(u8 **)(gTerrainMap + 0x10);
    return base + *(u16 *)(table + 2) + next * 8;
}