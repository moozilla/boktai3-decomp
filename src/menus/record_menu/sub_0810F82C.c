#include "engine/shared/global.h"
#include "engine/shared/engine.h"
#include "engine/shared/ram.h"
/* LinkLobbyPanel_Close: Hides panel sprites and clears background tilemaps before killing the singleton task */
#define LinkLobbyPanel_Close sub_0810F82C

void LinkLobbyPanel_HideSprites(void *);
void ScrollingDialogue_ClearScript(void);

void sub_0810F82C(void)
{
    if (gActiveRecordMenu) {
        LinkLobbyPanel_HideSprites(gActiveRecordMenu);
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        ScrollingDialogue_ClearScript();
        Task_Kill(gActiveRecordMenu);
    }
}
