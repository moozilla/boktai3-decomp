#include "global.h"

extern u32 gUnk_020000E0;
void ScrollingDialogue_Hide(void);
s32 ScrollingDialogue_HideImmediately(void)
{
    s32 r;
    if (gUnk_020000E0 != 0) {
        ScrollingDialogue_Hide();
        r = 0;
    } else
        r = -1;
    return r;
}
