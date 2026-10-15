#include "global.h"
extern u32 gUnk_020000B0;
void AnimData_SelectPaletteBank(void *, u32);
void SpritePool_SetPalette(u8 *p, u32 id)
{
    if (gUnk_020000B0)
        AnimData_SelectPaletteBank(p + 0x38, id);
}
