#include "global.h"
#include "engine.h"
/* LinkLobbyPanel_Close: Hides panel sprites and clears background tilemaps before killing the singleton task */
#define LinkLobbyPanel_Close sub_0810F82C

extern void *gUnk_020001D0;
void LinkLobbyPanel_HideSprites(void);
void ScrollingDialogue_ClearScript(void);

void sub_0810F82C(void)
{
    if (gUnk_020001D0) {
        LinkLobbyPanel_HideSprites();
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        ScrollingDialogue_ClearScript();
        Task_Kill(gUnk_020001D0);
    }
}
