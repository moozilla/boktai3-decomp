#include "global.h"

u32 *Text_GetGlyphCache(void);
s32 Text_FindCachedGlyph(u32 a, u32 b, u32 c, u32 d);
void Text_QueueTileUpload(s32 index, u32 *tile);

s32 Text_DrawGlyph(u32 *data, u32 glyph, u32 part, u32 color, u32 wide)
{
    u32 *cache = Text_GetGlyphCache();
    s32 index = Text_FindCachedGlyph(glyph, part, color, wide);
    u32 tile[8];
    u32 hi, lo, fill;
    s32 i;
    u32 *dst;
    u32 *src;

    if (index >= 0) {
        u32 *e = (u32 *)((index << 2) + (u32)cache);
        u32 v = *e;
        u32 cnt = v & 0x3FC00000;
        *e = (v & 0xC03FFFFF) | (cnt + 0x400000);
        return index;
    }
    for (i = 0; i <= 0xFF; cache++, i++) {
        if (!(*cache & 0x3FC00000))
            break;
    }
    if (i == 0x100)
        return -1;
    index = i;
    *cache = glyph | (part << 16) | (color << 18) | (wide << 20) | 0x400000;
    fill = color * 0x01010101;
    hi = fill << 6;
    lo = fill << 2;
    src = data;
    {
        u32 mhi = 0xF0F0F0F0;
        u32 mlo = 0x0F0F0F0F;
        dst = tile;
        for (i = 7; i >= 0; i--) {
            u32 w = *src++;
            *dst++ = (((w & mhi) | hi) & mhi) | (((w & mlo) | lo) & mlo);
        }
    }
    Text_QueueTileUpload(index, tile);
    return index;
}
