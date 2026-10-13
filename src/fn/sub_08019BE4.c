#include "global.h"
extern u32 gUnk_020000B0;
void AnimData_SelectTiles(void *, u32);
void SpritePool_SetPalette(u8 *p, u32 id)
{
    if (gUnk_020000B0)
        AnimData_SelectTiles(p + 0x38, id);
}
