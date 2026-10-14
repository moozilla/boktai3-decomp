#include "modifier_list.h"

void sub_08216494(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void ModifierMenu_ResetBackground(struct ModifierMenu *menu)
{
    sub_08216494(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
}
