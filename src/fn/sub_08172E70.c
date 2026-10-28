#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "scene_hud_display.h"
#include "input.h"
u32 SceneInput_NavigateGridKeys(void *, u32);
void SceneHud_SetActionCursorPosition(void *, u32);
void SceneHud_ShowSelectionDescription(void *);
void MenuDisplay_UpdateActionPrompt(void *, u32);
s32 PauseMenu_CanSwapCategoryEntries(void *, u32, u32, u32);
void Sound_PlaySong(u32);
s32 SolarFruit_GetInventorySlot(u32);
void SceneHud_LoadActionMotion(void *, u32, u32, u32, u32);
void sub_0816676C(void *, u32);
u32 GameState_GetUnlockEntry(u32);
void SceneHud_FinishActionTransfer(void);
void SceneHud_UpdateActionChoice(void);
void Hud_SetInteractionCallback(void *, void (*)(void), u32);
void SceneHud_ConfigureActionGrid(void *, u32, u32);
void Hud_RestoreGridSelection(void *);
void Hud_SetGridPosition(void *, u32, u32);
// confidence=low: confirms transfers between fruit or unlock grid entries.
void SceneHud_UpdateActionTransferSelection(struct SceneHudState *p)
{
    u8 *grid = p->actionInput;
    u8 *selected;
    u32 value, mode;
    if (SceneInput_NavigateGridKeys(grid, 0xf0)) {
        SceneHud_SetActionCursorPosition((u8 *)p + 0x1620, p->actionCursor);
        SceneHud_ShowSelectionDescription(p);
    }
    selected = &p->actionCursor;
    value = *selected;
    mode = 1;
    if (value <= 7)
        mode = 0;
    MenuDisplay_UpdateActionPrompt(p, mode);
    if (gPadState[1] & 1) {
        u8 *source;
        u32 current;
        if (p->actionInput[1] <= 1) {
            current = *selected;
            source = &p->actionSelection;
            if (!PauseMenu_CanSwapCategoryEntries(p, current, *source, 0))
                goto invalid;
            Sound_PlaySong(0x38a);
            {
                s32 slot = SolarFruit_GetInventorySlot(*source);
                register s32 negative asm("r7");
                asm("" : "=r"(negative));
                negative = 1;
                negative = -negative;
                if (slot != negative)
                    SceneHud_LoadActionMotion(p, 0, *source, *selected, 0);
                if (SolarFruit_GetInventorySlot(*selected) != negative)
                    SceneHud_LoadActionMotion(p, 1, *selected, *source, 0);
                sub_0816676C(p, 0);
            }
        } else {
            current = *selected;
            source = &p->actionSelection;
            if (!PauseMenu_CanSwapCategoryEntries(p, current, *source, 1)) {
            invalid:
                Sound_PlaySong(0x192);
                return;
            }
            if (GameState_GetUnlockEntry(*source - 8))
                SceneHud_LoadActionMotion(p, 0, *source, *selected, 1);
            if (GameState_GetUnlockEntry(*selected - 8))
                SceneHud_LoadActionMotion(p, 1, *selected, *source, 1);
            sub_0816676C(p, 1);
        }
        p->categoryWidgets[p->actionSelection + 10].flags |= 1;
        p->categoryWidgets[p->actionCursor + 10].flags |= 1;
        Hud_SetInteractionCallback(p, SceneHud_FinishActionTransfer, 1);
    } else if (gPadState[1] & 2) {
        Hud_SetInteractionCallback(p, SceneHud_UpdateActionChoice, 1);
        SceneHud_ConfigureActionGrid(grid, 0, 0);
        p->categoryWidgets[6].flags |= 1;
        Hud_RestoreGridSelection(grid);
        Hud_SetGridPosition(grid, p->actionInput[1], p->actionInput[2]);
        SceneHud_SetActionCursorPosition((u8 *)p + 0x1620, *selected);
        Sound_PlaySong(0xde);
    }
}
