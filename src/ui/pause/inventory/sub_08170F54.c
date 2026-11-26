#include "global.h"
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void sub_08170F54(u8 *p)
{
    u32 *q = (u32 *)(p + 0x2A40);
    q[2] |= 1;
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
}
