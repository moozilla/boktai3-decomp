#include "global.h"
#include "engine.h"
/* SaveMenu_Close: Clears background tilemaps and kills the active menu task */
#define SaveMenu_Close sub_08110A98

extern void *gUnk_020001D4;
void LinkScorePanel_HideSprites(void);
void ScrollingDialogue_ClearScript(void);

void sub_08110A98(void)
{
    if (gUnk_020001D4) {
        LinkScorePanel_HideSprites();
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        ScrollingDialogue_ClearScript();
        Task_Kill(gUnk_020001D4);
    }
}
