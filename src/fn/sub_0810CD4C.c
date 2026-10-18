#include "global.h"
#define LinkPanel_CreateTextWindows sub_0810CD4C
#define TextWindow_Create           sub_08033690
struct LinkPanel {
    u8 pad0[0x13F0];
    const u8 *script;
    u8 pad13F4[9];
    u8 selection;
    u8 pad13FE[2];
    s32 leftWindow;
    s32 rightWindow;
    s32 titleWindow;
};
s32 sub_08033690(s32, s32, s32, s32);
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_StartPlayback(s32);
static inline s32 clampIndex(s32 index, s32 low, s32 high)
{
    register s32 result = index;
    if (index > high)
        result = high;
    else if (index < low)
        result = low;
    return result;
}
void sub_0810CD4C(struct LinkPanel *p)
{
    s32 index;
    p->titleWindow = sub_08033690(1, 3, 28, 2);
    p->leftWindow = sub_08033690(1, 6, 10, 2);
    p->rightWindow = sub_08033690(19, 6, 10, 2);
    index = p->selection - 5;
    index = clampIndex(index, 0, 5);
    TextWindow_SetScript(p->titleWindow, p->script);
    TextWindow_Refresh(p->titleWindow, index);
    TextWindow_StartPlayback(p->titleWindow);
}
