#include "global.h"
#include "scene_hud_display.h"
#include "render.h"
#define gMenuMode gFadeMultiplier
void Hud_InitSelectionBounds(u32, u32);
s32 MessageWindow_Clear(void);
u32 ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void Hud_BeginRetryPanel(void *);
void PauseMenu_RestoreDisplayState(void *);
void SceneHud_FinishPageExit(void *);
void Hud_SetUpdateCallback(void *, void (*)(void *));
void SceneHud_FadeOutPage(struct SceneHudState *p)
{
    if (p->pageFadeTimer <= 15)
        Hud_InitSelectionBounds(0x40 - (p->pageFadeTimer << 2), 0x1084);
    else
        Hud_InitSelectionBounds(0, 0x1084);
    if (p->pageFadeTimer == 15) {
        MessageWindow_Clear();
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_RequestHide();
    }
    if (++p->pageFadeTimer > 18) {
        if (p->pageReturnToParent) {
            p->pageReturnToParent = 0;
            Hud_BeginRetryPanel(p);
        } else {
            PauseMenu_RestoreDisplayState(p);
            gFadeMultiplier = p->savedMenuMode;
            Hud_SetUpdateCallback(p, SceneHud_FinishPageExit);
        }
    }
}
