#include "global.h"
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_HideImmediately(void);
void Bg_ClearTilemap(s32);
void LinkScorePanel_HideSprites(void *);
void Registry1745_SetMenuRequest(s32);
void Task_Kill(void *);
void sub_08110128(void *task)
{
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_HideImmediately();
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    LinkScorePanel_HideSprites(task);
    Registry1745_SetMenuRequest(1);
    Task_Kill(task);
}
