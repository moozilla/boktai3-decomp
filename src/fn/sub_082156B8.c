#include "global.h"

void Text_ClearGlyphCache(void);
u32 Video_GetBackgroundMap(u32);
void Memory_Clear(u32, u32);

void Bg_ClearTilemap(u32 n)
{
    if (n == 0)
        Text_ClearGlyphCache();
    Memory_Clear(Video_GetBackgroundMap(n), 0x800);
}
