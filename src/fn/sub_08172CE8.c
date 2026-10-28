#include "global.h"
#include "scene_hud_display.h"
#include "input.h"
void Sound_PlaySong(u32);
int SceneInput_NavigateGridKeys(void *, int);
void SceneHud_SetActionCursorPosition(void *, u32);
void SceneHud_ShowSelectionDescription(void *);
void MenuDisplay_UpdateActionPrompt(void *, int);
void PauseInventory_ReturnToEquipmentSelection(void *);
void Hud_SaveGridSelection(void *);
void SceneHud_ConfigureActionGrid(void *, u32, u32);
void SceneHud_UpdateActionTransferSelection(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void SceneHud_UpdateActionChoice(struct SceneHudState *p)
{
    u8 *q = p->actionInput;
    u8 *cursor;
    u32 mode;
    u8 value;
    if (SceneInput_NavigateGridKeys(q, 0xf0)) {
        SceneHud_SetActionCursorPosition((u8 *)p + 0x1620, p->actionCursor);
        SceneHud_ShowSelectionDescription(p);
    }
    cursor = &p->actionCursor;
    value = *cursor;
    mode = 1;
    if (value <= 7)
        mode = 0;
    MenuDisplay_UpdateActionPrompt(p, mode);
    if (gPadState[1] & 2) {
        PauseInventory_ReturnToEquipmentSelection(p);
        Sound_PlaySong(0xde);
    } else if (1 & gPadState[1]) {
        Sound_PlaySong(0xdd);
        Hud_SaveGridSelection(q);
        p->categoryWidgets[6].flags &= ~1;
        SceneHud_SetActionCursorPosition((u8 *)p + 0x1680, p->actionSelection);
        if (*cursor <= 7)
            SceneHud_ConfigureActionGrid(q, 0, 1);
        else
            SceneHud_ConfigureActionGrid(q, 0, 2);
        Hud_SetInteractionCallback(p, SceneHud_UpdateActionTransferSelection, 1);
    }
}
