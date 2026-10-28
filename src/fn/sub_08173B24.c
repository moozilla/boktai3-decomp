#include "global.h"
#include "input.h"
void sub_0816618C(void *, s32, s32);
void Sound_PlaySong(s32);
void Hud_SaveSecondaryGridSelection(void *);
void SceneHud_GetActionIconPosition(void *, s32);
void MotionSequence_BeginResetPrompt(void *);
void PauseInventory_BeginItemUseSelection(void *, s32);
void PauseInventory_BeginItemSwapSelection(void *, s32);
void PauseInventory_BeginItemDiscardSelection(void *, s32);
void PauseMenu_ClearSpriteFlags(void *, s32, s32);
void PauseMenu_ResetItemCursor(void *);
s32 SceneInput_NavigateGridKeys(void *, s32);
void SceneHud_ShowEquipmentDetails(void *);
void MenuDisplay_UpdateActionPrompt(void *, s32);
void Hud_UpdateInventoryMenuInput(u8 *p)
{
    sub_0816618C(p, 8, 32);
    p[0xa58] = 0;
    if (1 & gPadState[1]) {
        u8 *q;
        Sound_PlaySong(0xdd);
        Hud_SaveSecondaryGridSelection(p + 0x4384);
        {
            u8 *a = p + 0x15c0;
            q = p + 0x4387;
            SceneHud_GetActionIconPosition(a, *q);
        }
        if (*q == 0x27) {
            MotionSequence_BeginResetPrompt(p);
            return;
        }
        if (*q == 0x20)
            PauseInventory_BeginItemUseSelection(p, 1);
        else if (*q == 0x26)
            PauseInventory_BeginItemSwapSelection(p, 1);
        else if (*q == 0x28)
            PauseInventory_BeginItemDiscardSelection(p, 1);
        PauseMenu_ClearSpriteFlags(p, 5, 1);
        PauseMenu_ResetItemCursor(p);
        SceneHud_GetActionIconPosition(p + 0x1620, p[0x4387]);
    } else if (SceneInput_NavigateGridKeys(p + 0x4384, 0xc0)) {
        SceneHud_GetActionIconPosition(p + 0x15c0, p[0x4387]);
        SceneHud_ShowEquipmentDetails(p);
        MenuDisplay_UpdateActionPrompt(p, 2);
    }
}
