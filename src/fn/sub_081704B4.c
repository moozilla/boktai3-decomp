#include "global.h"
#include "scene_hud_display.h"
#include "input.h"
int SceneInput_NavigateGridKeys(void *, int);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_ShowActionPreview(void *);
void MenuDisplay_UpdateActionPrompt(void *, int);
u32 PauseMenu_CanSwapCategoryEntries(void *, u32, u32, u32);
s32 Inventory_GetQuantityByStorage(u32, u32);
void SceneHud_BeginActionIconMotion(void *, int, int, int);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void SceneHud_FinishActionSlide(void);
void PauseMenu_OnSwapSelectionUpdate(void);
void Hud_RestoreGridSelection(void *);
void Hud_SetGridPosition(void *, int, int);
void SceneHud_UpdateActionSwap(struct SceneHudState *p)
{
    u8 *input = p->actionInput;
    u8 *cursor, *selection, *category;
    struct SceneInputWidget *widget;
    if (SceneInput_NavigateGridKeys(input, 240)) {
        SceneHud_GetActionIconPosition((u8 *)p + 0x1620, p->actionCursor);
        SceneHud_ShowActionPreview(p);
    }
    MenuDisplay_UpdateActionPrompt(p, 4);
    if (gPadState[1] & 1) {
        cursor = &p->actionCursor;
        if (!PauseMenu_CanSwapCategoryEntries(p, *cursor, *(selection = &p->actionSelection), 4)) {
            Sound_PlaySong(0x192);
        } else {
            Sound_PlaySong(0x38A);
            category = &p->actionCategory;
            if (Inventory_GetQuantityByStorage(*category, *selection) >= 0)
                SceneHud_BeginActionIconMotion(p, 0, *selection, *cursor);
            if (Inventory_GetQuantityByStorage(*category, *cursor) >= 0)
                SceneHud_BeginActionIconMotion(p, 1, *cursor, *selection);
            p->categoryWidgets[*selection + 10].flags |= 1;
            p->categoryWidgets[*cursor + 10].flags |= 1;
            Hud_SetInteractionCallback(p, SceneHud_FinishActionSlide, 1);
        }
    } else if (gPadState[1] & 2) {
        Hud_SetInteractionCallback(p, PauseMenu_OnSwapSelectionUpdate, 1);
        Hud_RestoreGridSelection(input);
        Hud_SetGridPosition(input, p->actionInput[1], p->actionInput[2]);
        SceneHud_GetActionIconPosition((u8 *)p + 0x1620, p->actionCursor);
        widget = &p->categoryWidgets[6];
        widget->flags |= 1;
        SceneHud_ShowActionPreview(p);
        Sound_PlaySong(222);
    }
}
