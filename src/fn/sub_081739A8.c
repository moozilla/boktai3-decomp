#include "global.h"
void SceneHud_RefreshEquipmentIcons(void *, int);
void sub_08173D2C(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void Hud_SetGridPosition(void *, int, int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void PauseMenu_AddSpriteFlags(void *, int, int);
void sub_0816CDB8(void *, int, int);
void MenuDisplay_UpdateActionPrompt(void *, int);
void SceneHud_ShowEquipmentDetails(void *);
void PauseInventory_BeginItemSwapSelection(u8 *p, int flag)
{
    SceneHud_RefreshEquipmentIcons(p, 0);
    Hud_SetInteractionCallback(p, sub_08173D2C, 1);
    if (flag)
        Hud_SetGridPosition(p + 0x4384, 0, 0);
    MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, 0);
    PauseMenu_AddSpriteFlags(p, 6, 1);
    sub_0816CDB8(p + 0x4384, 1, 0);
    MenuDisplay_UpdateActionPrompt(p, 2);
    SceneHud_ShowEquipmentDetails(p);
}
