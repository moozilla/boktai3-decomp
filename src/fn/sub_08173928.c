#include "global.h"
void SceneHud_RefreshEquipmentIcons(void *, int);
void sub_08173C0C(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_0816CDB8(void *, int, int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void PauseMenu_ResetItemCursor(void *);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_ShowEquipmentDetails(void *);
void sub_0816701C(void *, int, int, int, int, int);
void MenuDisplay_UpdateActionPrompt(void *, int);
void PauseInventory_BeginItemUseSelection(u8 *p, int flag)
{
    int z;
    u8 *a;
    SceneHud_RefreshEquipmentIcons(p, 0);
    Hud_SetInteractionCallback(p, sub_08173C0C, 1);
    sub_0816CDB8(p + 0x4384, 1, 0);
    z = 0;
    MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, z);
    if (flag)
        PauseMenu_ResetItemCursor(p);
    a = p + 0x1620;
    SceneHud_GetActionIconPosition(a, p[0x4387]);
    SceneHud_ShowEquipmentDetails(p);
    sub_0816701C(p, 0, 1, 0, z, z);
    MenuDisplay_UpdateActionPrompt(p, 2);
}
