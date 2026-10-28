#include "global.h"
#include "input.h"
/* Tentative: consumable grid selection for the pause inventory discard flow. */
int SceneInput_NavigateGridKeys(void *, int);
void SceneHud_GetActionIconPosition(void *, int);
void PauseInventory_RefreshConsumableDescription(void *);
void MenuDisplay_UpdateActionPrompt(void *, int);
void Hud_SaveGridSelection(void *);
s32 Inventory_GetConsumableId(u32);
s32 GameState_IsConsumableSlotEquipped(u32);
void Sound_PlaySong(u32);
void PauseMenu_AddSpriteFlags(void *, s32, s32);
void PauseMenu_OpenPrompt(void *, s32, s32);
void PauseMenu_SetConfirmationIndicator(void *, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void PauseInventory_ReturnToConsumableActions(void *);
void MotionSequence_ResolveSelection(void);
void sub_08171D5C(u8 *p)
{
    u8 *grid = p + 0x4384;
    if (SceneInput_NavigateGridKeys(grid, 0xF0)) {
        SceneHud_GetActionIconPosition(p + 0x1620, *(p + 0x4387));
        PauseInventory_RefreshConsumableDescription(p);
        MenuDisplay_UpdateActionPrompt(p, 3);
    }
    if (gPadState[1] & 1) {
        u8 *slot;
        s32 equipped;
        Hud_SaveGridSelection(grid);
        slot = p + 0x438A;
        if (Inventory_GetConsumableId(*slot) == -1 ||
            (equipped = GameState_IsConsumableSlotEquipped(*slot)) != 0) {
            Sound_PlaySong(0x192);
        } else {
            PauseMenu_AddSpriteFlags(p, 0x3B, 1);
            PauseMenu_OpenPrompt(*(void **)(p + 0x5C), 2, 1);
            PauseMenu_SetConfirmationIndicator(p, 0);
            Hud_SetInteractionCallback(p, MotionSequence_ResolveSelection, 1);
            MenuDisplay_SetPaletteOverrides(p, 0, 0, 1, equipped);
        }
    } else if (gPadState[1] & 2) {
        Sound_PlaySong(0xDE);
        PauseInventory_ReturnToConsumableActions(p);
    }
}
