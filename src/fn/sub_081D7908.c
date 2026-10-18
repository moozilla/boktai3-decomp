#include "global.h"
void SpriteDrawable_SelectTileFrame(u8 *, u32, u32);
void Effect_SetPalette(u8 *, u32);
void Task4E01_SetTrailSpriteResources(u8 *p, u32 b)
{
    u8 *q = p + 0x48;
    s32 i = 3;
    do {
        SpriteDrawable_SelectTileFrame(q, *(u32 *)(p + 0x44), b);
        Effect_SetPalette(q, 1);
        q += 0x30;
        i--;
    } while (i >= 0);
}
