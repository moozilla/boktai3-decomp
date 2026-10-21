#define SceneHud_ShowWidget PauseMenu_ClearSpriteFlags
#define SceneHud_HideWidget PauseMenu_AddSpriteFlags
#include "global.h"
#include "scene_hud_display.h"
void SceneHud_RefreshEquipmentIcons(void *, int);
void Hud_RestoreSecondaryGridSelection(void *);
void SceneInput_BuildActionGrid_08173508(void *, int);
void PauseMenu_ClearSpriteFlags(void *, int, int);
void PauseMenu_AddSpriteFlags(void *, int, int);
void InventoryPanel_ShowStatusIcons(void *);
void MenuDisplay_UpdateActionPrompt(void *, int);
void sub_08166534(void *, int, int);
void PauseMenu_RefreshEquipmentPanel(void *);
void Hud_UpdateInventoryMenuInput(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void InventoryPanel_UpdateSelectedUnlock(void *, int, int, int, int);
void InventoryPanel_UpdateSelectedEquipment(void *, int, int, int, int);
void sub_08173450(void *);
void SceneHud_BeginEquipmentSelection(struct SceneHudState *p)
{
    void *q;
    int z;
    SceneHud_RefreshEquipmentIcons(p, 0);
    q = p->actionInput;
    Hud_RestoreSecondaryGridSelection(q);
    SceneInput_BuildActionGrid_08173508(q, 0xe1);
    PauseMenu_ClearSpriteFlags(p, 4, 1);
    PauseMenu_AddSpriteFlags(p, 6, 1);
    PauseMenu_AddSpriteFlags(p, 5, 1);
    InventoryPanel_ShowStatusIcons(p);
    MenuDisplay_UpdateActionPrompt(p, 2);
    sub_08166534(p, 10, 6);
    PauseMenu_RefreshEquipmentPanel(p);
    Hud_SetInteractionCallback(p, Hud_UpdateInventoryMenuInput, 0);
    z = 0;
    MenuDisplay_SetPaletteOverrides(p, 0, 1, 0, z);
    InventoryPanel_UpdateSelectedUnlock(p, 8, 3, 0, z);
    InventoryPanel_UpdateSelectedEquipment(p, 8, 4, 0, z);
    sub_08173450(p);
}
