#include "global.h"
#include "input.h"
int SceneInput_NavigateGridKeys(void *, int);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_ShowActionPreview(void *);
void MenuDisplay_UpdateActionPrompt(void *, int);
void SceneHud_ReturnToActionGrid(void *);
void Sound_PlaySong(int);
void Hud_SaveGridSelection(void *);
void SceneHud_UpdateActionSwap(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void PauseMenu_OnSwapSelectionUpdate(u8 *p)
{
    u8 *q = p + 0x4384;
    if (SceneInput_NavigateGridKeys(q, 0xf0)) {
        u8 *a = p + 0x1620;
        SceneHud_GetActionIconPosition(a, p[0x4387]);
        SceneHud_ShowActionPreview(p);
    }
    MenuDisplay_UpdateActionPrompt(p, 4);
    if (gPadState[1] & 2) {
        SceneHud_ReturnToActionGrid(p);
        Sound_PlaySong(0xde);
    } else if (1 & gPadState[1]) {
        u8 *a;
        Sound_PlaySong(0xdd);
        Hud_SaveGridSelection(q);
        *((u32 *)(p + 0x1668)) &= ~1;
        a = p + 0x1680;
        SceneHud_GetActionIconPosition(a, p[0x438a]);
        Hud_SetInteractionCallback(p, SceneHud_UpdateActionSwap, 1);
    }
}
