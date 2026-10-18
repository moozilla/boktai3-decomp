#include "global.h"
void sub_08172638(u8 *);
void Hud_RestoreSecondaryGridSelection(void *);
void SceneHud_ConfigureActionGrid(void *, u32, u32);
void PauseMenu_ClearSpriteFlags(u8 *, u32, u32);
void PauseMenu_AddSpriteFlags(u8 *, u32, u32);
void SceneHud_SetActionCursorPosition(void *, u32);
void MenuDisplay_UpdateActionPrompt(u8 *, u32);
void PauseMenu_RefreshEquipmentPanel(u8 *);
void SelectionPanel_ResetSecondarySelection(u8 *);
void PauseMenu_SetPromptIconPositions(u8 *, u32, u32);
void Hud_SetInteractionCallback(u8 *, void *, u32);
void sub_081731E8(void);
void MenuDisplay_SetPaletteOverrides(u8 *, u32, u32, u32, u32);
u32 InventoryPanel_UpdateSelectedUnlock(u8 *, u32, u32, u32, u32);
void InventoryPanel_SetEntryRow(u8 *, u32, u32, u32, u32, u32);
void SceneHud_RefreshEquippedActionIcons(u8 *);
void PauseInventory_ReturnToEquipmentSelection(u8 *s)
{
    u8 *p, *q;
    u32 zero, result;
    sub_08172638(s);
    p = s + 0x4384;
    Hud_RestoreSecondaryGridSelection(p);
    SceneHud_ConfigureActionGrid(p, 0x61, 0);
    PauseMenu_ClearSpriteFlags(s, 4, 1);
    PauseMenu_AddSpriteFlags(s, 6, 1);
    PauseMenu_AddSpriteFlags(s, 5, 1);
    q = s + 0x15C0;
    p = s + 0x4387;
    SceneHud_SetActionCursorPosition(q, *p);
    {
        u8 v = *p;
        u32 r = 1;
        if (v <= 7)
            r = 0;
        MenuDisplay_UpdateActionPrompt(s, r);
    }
    PauseMenu_RefreshEquipmentPanel(s);
    SelectionPanel_ResetSecondarySelection(s);
    PauseMenu_SetPromptIconPositions(s, 0, 0);
    Hud_SetInteractionCallback(s, sub_081731E8, 0);
    {
        struct Flags {
            u32 pad[2], flags;
        };
        struct Flags *f = (struct Flags *)(s + 0x2A40);
        f->flags |= 1;
    }
    MessageWindow_Clear();
    ScrollingDialogue_RequestHide();
    MenuDisplay_SetPaletteOverrides(s, 0, 1, 0, zero = 0);
    result = InventoryPanel_UpdateSelectedUnlock(s, 0, 0, 1, zero);
    InventoryPanel_SetEntryRow(s, 3, 14, result, zero, 0x47);
    SceneHud_RefreshEquippedActionIcons(s);
}
