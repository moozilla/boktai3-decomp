#include "global.h"
#define Script_GetArgumentPc  Script_GetTextSourcePc
#define TextWindow_SetMode    MessageWindow_SetDisplayMode
#define TextWindow_SetPalette MessageWindow_SetPalette
struct Storage {
    u8 pad0[0x1734];
    u32 optionI, optionW, optionA, optionS;
};
s32 Script_SeekToKeyword(u32);
u32 Script_GetTextSourcePc(void);
void ScrollingDialogue_RequestShow(void);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
s32 MessageWindow_SetDisplayMode(s32);
void MessageWindow_SetPalette(int);
void Storage_InitTextOptions(struct Storage *p)
{
    if (Script_SeekToKeyword('i'))
        p->optionI = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('w'))
        p->optionW = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('a'))
        p->optionA = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('s'))
        p->optionS = Script_GetTextSourcePc();
    ScrollingDialogue_RequestShow();
    MessageWindow_SetBounds(0, 16, 30, 2);
    MessageWindow_SetDisplayMode(1);
    MessageWindow_SetPalette(0x81b);
}
