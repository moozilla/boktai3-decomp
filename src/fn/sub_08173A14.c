#include "global.h"
void SceneHud_RefreshEquipmentIcons(void *, int);
void sub_081740CC(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void PauseMenu_ClearSpriteFlags(void *, int, int);
void PauseMenu_ResetItemCursor(void *);
void sub_0816CDB8(void *, int, int);
void MenuDisplay_UpdateActionPrompt(void *, int);
void SceneHud_ShowEquipmentDetails(void *);
void PauseInventory_BeginItemDiscardSelection(u8 *p, int flag)
{
    SceneHud_RefreshEquipmentIcons(p, 1);
    Hud_SetInteractionCallback(p, sub_081740CC, 1);
    MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, 0);
    PauseMenu_ClearSpriteFlags(p, 5, 1);
    if (flag)
        PauseMenu_ResetItemCursor(p);
    sub_0816CDB8(p + 0x4384, 1, 0);
    MenuDisplay_UpdateActionPrompt(p, 2);
    SceneHud_ShowEquipmentDetails(p);
}
