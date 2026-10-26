#include "global.h"
#include "engine.h"
#include "ram.h"
/* SaveMenu_Close: Clears background tilemaps and kills the active menu task */
#define SaveMenu_Close sub_08110A98

void LinkScorePanel_HideSprites(void);
void ScrollingDialogue_ClearScript(void);

void sub_08110A98(void)
{
    if (gSaveMenu) {
        LinkScorePanel_HideSprites();
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        ScrollingDialogue_ClearScript();
        Task_Kill(gSaveMenu);
    }
}
