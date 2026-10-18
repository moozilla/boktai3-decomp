#include "global.h"

void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_HideImmediately(void);
void Bg_ClearTilemap(s32);
void Registry3C9F_HideDisplayObjects(void *);
void Registry1745_SetMenuRequest(s32);
void Task_Kill(void *);

void Registry3C9F_Close(void *p)
{
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_HideImmediately();
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    Registry3C9F_HideDisplayObjects(p);
    Registry1745_SetMenuRequest(1);
    Task_Kill(p);
}
