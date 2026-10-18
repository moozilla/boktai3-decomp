#include "global.h"

extern void *gUnk_020001C4;
void RegistryF246_HideAnimations(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_HideImmediately(void);
void Bg_ClearTilemap(s32);
void Task_Kill(void *);

void RegistryF246_Close(void)
{
    if (gUnk_020001C4 != 0) {
        RegistryF246_HideAnimations();
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_HideImmediately();
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        Task_Kill(gUnk_020001C4);
    }
}
