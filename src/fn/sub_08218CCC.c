#include "global.h"

u32 *Text_GetGlyphCache(void);

s32 Text_FindCachedGlyph(u32 a, u32 b, u32 c, u32 d)
{
    u32 key = (a | (b << 16) | (c << 18) | (d << 20)) & 0x3FFFFF;
    u32 *p = Text_GetGlyphCache();
    s32 i = 0;
    do {
        u32 v = *p;
        if ((v & 0x3FFFFF) == key && (v & 0x3FC00000) != 0)
            return i;
        p++;
        i++;
    } while (i <= 0xFF);
    return -1;
}
