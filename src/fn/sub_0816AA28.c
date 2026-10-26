#include "global.h"
#include "render.h"
void Hud_InitSelectionBounds(s32, s32);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void PauseMenu_RestoreDisplayState(u8 *);
void Hud_SetUpdateCallback(u8 *, void (*)(u8 *));
void SceneHud_FinishPageExit(u8 *);

void sub_0816AA28(u8 *hud)
{
    u16 *timer;
    if (*(u16 *)(hud + 0xa4c) <= 15)
        Hud_InitSelectionBounds(0x40 - *(u16 *)(hud + 0xa4c) * 4, 0x1084);
    else
        Hud_InitSelectionBounds(0, 0x1084);
    timer = (u16 *)(hud + 0xa4c);
    if (*timer == 15) {
        MessageWindow_Clear();
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_RequestHide();
    }
    if (++*timer > 18) {
        PauseMenu_RestoreDisplayState(hud);
        gFadeMultiplier = *(u16 *)(hud + 0xa12);
        Hud_SetUpdateCallback(hud, SceneHud_FinishPageExit);
    }
}
