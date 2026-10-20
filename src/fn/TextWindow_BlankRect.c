#include "global.h"

u32 Video_GetBackgroundMap(u32);
s32 Text_ReleaseCachedGlyph(u32);

void TextWindow_BlankRect(u32 x, u32 y, u32 w, u32 h)
{
    u16 *map;
    u32 xEnd, yEnd, x0, left;

    map = (u16 *)Video_GetBackgroundMap(0);
    xEnd = (x0 = x) + w;
    yEnd = h + y;
    for (; y < yEnd; y++) {
        left = x0;
        for (x = left; x < xEnd; x++) {
            u16 *p = (u16 *)(((y * 32 + x) * 2) + (u32)map);
            if (Text_ReleaseCachedGlyph(*p & 0x3FF) != 0)
                *p = 0xF001;
        }
    }
}
