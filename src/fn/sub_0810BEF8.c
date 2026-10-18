#define TextWindow_Show TextWindow_StartPlayback
#include "global.h"
struct SlotMenu {
    u8 unknown0[0xF48];
    const u8 *script;
    u8 unknownF4C[0x14];
    s32 firstWindow;
    s32 secondWindow;
    s32 mainWindow;
};
s32 TextWindow_Create(s32, s32, s32, s32);
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_StartPlayback(s32);
void SlotMenu_CreateTextWindows(struct SlotMenu *p)
{
    p->mainWindow = TextWindow_Create(1, 3, 28, 2);
    p->firstWindow = TextWindow_Create(1, 6, 10, 2);
    p->secondWindow = TextWindow_Create(1, 6, 10, 2);
    TextWindow_SetScript(p->mainWindow, p->script);
    TextWindow_Refresh(p->mainWindow, 0);
    TextWindow_Show(p->mainWindow);
}
