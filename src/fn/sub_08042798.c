#include "display_animation.h"
void Anim_SelectPaletteBank(void *, u32);
s32 DisplayAnimation_SelectPaletteBank(struct DisplayAnimation *display, u32 index)
{
    if (!display->kind)
        return -1;
    if (display->kind == 1)
        AnimData_SelectPaletteBank(&display->resource, index);
    else
        Anim_SelectPaletteBank((u8 *)display + 0x2C, index);
    return 0;
}
