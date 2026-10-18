#include "global.h"
#include "scene_hud_display.h"
#define SceneHud_FadeOutPage sub_08176B00
#define gMenuMode            gUnk_030051D8
extern u32 gUnk_030051D8;
void Hud_InitSelectionBounds(u32, u32);
s32 MessageWindow_Clear(void);
u32 ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void Hud_BeginRetryPanel(void *);
void sub_08176718(void *);
void sub_08176BA4(void *);
void Hud_SetUpdateCallback(void *, void (*)(void *));
void sub_08176B00(struct SceneHudState *p)
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
            sub_08176718(p);
            gUnk_030051D8 = p->savedMenuMode;
            Hud_SetUpdateCallback(p, sub_08176BA4);
        }
    }
}
