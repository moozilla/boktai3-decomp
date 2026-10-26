#include "global.h"
#include "ram.h"
void AnimData_SelectPaletteBank(void *, u32);
void SpritePool_SetPalette(u8 *p, u32 id)
{
    if (gSpritePoolManager)
        AnimData_SelectPaletteBank(p + 0x38, id);
}
