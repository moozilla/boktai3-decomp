#include "global.h"
#define SpritePool_SetPalette sub_08019BE4
extern u32 gUnk_020000B0;
void sub_08215284(void *, u32);
void SpritePool_SetPalette(u8 *p, u32 id)
{
    if (gUnk_020000B0)
        sub_08215284(p + 0x38, id);
}
