#include "global.h"
#include "engine/engine.h"
#include "link/lobby/record_menu.h"

void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestShow(void);
void ScrollingDialogue_HideImmediately(void);
void LinkLobbyPanel_HideSprites(struct RecordMenu *menu);
void LinkSession_SetMenuRequest(u32 request);

void sub_0810F254(void)
{
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestShow();
}

void sub_0810F264(void)
{
}

void sub_0810F268(struct RecordMenu *menu)
{
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_HideImmediately();
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    LinkLobbyPanel_HideSprites(menu);
    LinkSession_SetMenuRequest(2);
    if (menu->exitScript) {
        Script_Call(menu->exitScript, NULL);
        Task_Kill(menu);
    }
}

void sub_0810F2B8(u8 *self)
{
    if (*(u16 *)(self + 0x1500) != 0) {
        Script_Call(*(u16 *)(self + 0x1500), 0);
        Task_Kill(self);
    }
}
