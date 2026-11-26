#include "global.h"
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void sub_08173450(u8 *p)
{
    u32 *q = (u32 *)(p + 0x2A40);
    q[2] |= 1;
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
}
