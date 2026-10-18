#include "global.h"
#include "engine.h"

extern void *gUnk_020001C8;
void Registry30AC_HideDisplay(void);
void ScrollingDialogue_ClearScript(void);

void sub_0810D554(void)
{
    if (gUnk_020001C8) {
        Registry30AC_HideDisplay();
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        ScrollingDialogue_ClearScript();
        Task_Kill(gUnk_020001C8);
    }
}
