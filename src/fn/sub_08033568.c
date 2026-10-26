#include "global.h"

extern u8 *gScrollingDialogue;

s32 ScrollingDialogue_RequestShow(void)
{
    s32 ret;

    if (gScrollingDialogue) {
        gScrollingDialogue[0x17D] = 1;
        ret = 0;
    } else {
        ret = -1;
    }
    return ret;
}
