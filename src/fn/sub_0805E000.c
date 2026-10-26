#include "scene_hud_display.h"
void SceneHud_SetKeyframePaletteBank(struct SceneInputWidget *target, u32 bank)
{
    target->paletteBank = bank;
    target->palette = (u16 *)(gPaletteBankData + (target->paletteBank << 5));
}
