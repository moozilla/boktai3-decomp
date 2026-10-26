#include "global.h"
#include "ram.h"

void ScrollingDialogue_Hide(void);
s32 ScrollingDialogue_HideImmediately(void)
{
    s32 r;
    if (gScrollingDialogue != 0) {
        ScrollingDialogue_Hide();
        r = 0;
    } else
        r = -1;
    return r;
}
