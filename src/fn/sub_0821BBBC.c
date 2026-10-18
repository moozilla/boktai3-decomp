#define Terrain_BuildRowOffsets sub_0821BBBC
#define gTerrainMap             gUnk_030052F4
#include "global.h"
#include "engine.h"
void sub_0821BBBC(void)
{
    s32 width = *(s16 *)(gUnk_030052F4->cells + 4);
    u16 *row = gUnk_030052F4->rows;
    s32 offset = 0;
    s32 i = 0;
    for (; i < *(s16 *)(gUnk_030052F4->cells + 6); i++) {
        row[i] = offset;
        offset += width;
    }
}
