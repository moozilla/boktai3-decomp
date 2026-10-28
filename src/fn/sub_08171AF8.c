#include "global.h"
#include "scene_hud_display.h"
// MATCHING_CONCESSION: register-pinning
void Hud_SetInteractionCallback(void *, void (*)(void), int);
u32 PauseMenu_CanSwapCategoryEntries(void *, u32, u32, u32);
void MenuDisplay_UpdateActionPrompt(void *, int);
int SceneInput_NavigateGridKeys(void *, int);
void Hud_RestoreGridSelection(u8 *);
void Hud_SetGridPosition(void *, int, int);
void SceneHud_GetActionIconPosition(void *, int);
void PauseInventory_GetConsumableCursorPosition(void *, u8 *);
void PauseInventory_RefreshConsumableDescription(void *);
s32 Inventory_GetConsumableId(u32);
void sub_0816676C(void *, s32);
void SceneHud_PrepareActionTransferToSlot(void *, s32, s32, s32);
void PauseInventory_UpdateConsumableSwapAnimation(void);
void PauseInventory_UpdateConsumableOptionSelection(void);
void SceneHud_UpdateActionSwapSelection(struct SceneHudState *p)
{
    u8 *input = p->actionInput;
    u16 one;
    u32 keys;
    u32 confirm;
    if (SceneInput_NavigateGridKeys(input, 0xF0)) {
        SceneHud_GetActionIconPosition(&p->categoryWidgets[5].position, p->actionCursor);
        PauseInventory_RefreshConsumableDescription(p);
        MenuDisplay_UpdateActionPrompt(p, 3);
    }
    keys = gPadState[1];
    one = 1;
    confirm = one;
    asm("" : "+r"(confirm));
    confirm &= keys;
    if (confirm) {
        u8 *cursor = &p->actionCursor;
        u32 current = *cursor;
        u8 *selection = &p->actionSelection;
        if (!PauseMenu_CanSwapCategoryEntries(p, current, *selection, 3)) {
            Sound_PlaySong(0x192);
            return;
        }
        Sound_PlaySong(0x38A);
        if (Inventory_GetConsumableId(*selection) >= 0)
            SceneHud_PrepareActionTransferToSlot(p, 0, *selection, *cursor);
        if (Inventory_GetConsumableId(*cursor) >= 0)
            SceneHud_PrepareActionTransferToSlot(p, 1, *cursor, *selection);
        p->categoryWidgets[*selection + 10].flags |= one;
        p->categoryWidgets[*cursor + 10].flags |= one;
        sub_0816676C(p, 3);
        Hud_SetInteractionCallback(p, PauseInventory_UpdateConsumableSwapAnimation, 1);
    } else {
        u32 two = 2;
        two &= keys;
        if (!two)
            return;
        Hud_SetInteractionCallback(p, PauseInventory_UpdateConsumableOptionSelection, 1);
        p->categoryWidgets[6].flags |= one;
        Hud_RestoreGridSelection(input);
        Hud_SetGridPosition(input, p->actionInput[1], p->actionInput[2]);
        PauseInventory_GetConsumableCursorPosition(&p->categoryWidgets[5].position, input);
        Sound_PlaySong(0xDE);
    }
}
