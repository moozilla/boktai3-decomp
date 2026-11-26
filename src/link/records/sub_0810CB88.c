#include "global.h"
#include "engine/ram.h"

void LinkPanelSlots_HideAnimations(void *);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_HideImmediately(void);
void Bg_ClearTilemap(s32);
void Task_Kill(void *);

void LinkPanelSlots_Close(void)
{
    if (gLinkPanel != 0) {
        LinkPanelSlots_HideAnimations(gLinkPanel);
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_HideImmediately();
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        Task_Kill(gLinkPanel);
    }
}
