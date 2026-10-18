#include "modifier_list.h"

void Bg_ResetScale(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void ModifierMenu_ResetBackground(struct ModifierMenu *menu)
{
    Bg_ResetScale(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
}
