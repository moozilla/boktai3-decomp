#include "scene_hud_display.h"
#define SceneHud_SetKeyframePaletteBank sub_0805E000
void SceneHud_SetKeyframePaletteBank(struct SceneInputWidget *target, u32 bank)
{
    target->paletteBank = bank;
    target->palette = (u16 *)(gUnk_030042E4 + (target->paletteBank << 5));
}
