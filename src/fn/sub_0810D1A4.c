#include "global.h"

void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestShow(void);
void ScrollingDialogue_HideImmediately(void);
void Bg_ClearTilemap(s32 bg);
void Registry30AC_HideDisplay(u8 *self);
void Registry1745_SetMenuRequest(u32 request);
void Script_Call(u32 script, void *args);
void Task_Kill(void *task);

void sub_0810D1A4(void)
{
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestShow();
}

void sub_0810D1B4(void)
{
}

void sub_0810D1B8(u8 *self)
{
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_HideImmediately();
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    Registry30AC_HideDisplay(self);
    Registry1745_SetMenuRequest(2);
    if (*(u16 *)(self + 0x1412) != 0) {
        Script_Call(*(u16 *)(self + 0x1412), 0);
        Task_Kill(self);
    }
}

void sub_0810D208(u8 *self)
{
    if (*(u16 *)(self + 0x1410) != 0) {
        Script_Call(*(u16 *)(self + 0x1410), 0);
        Task_Kill(self);
    }
}
