#include "global.h"
#include "ram.h"

s32 ScrollingDialogue_RequestShow(void)
{
    s32 ret;

    if (((u8 *)gScrollingDialogue)) {
        ((u8 *)gScrollingDialogue)[0x17D] = 1;
        ret = 0;
    } else {
        ret = -1;
    }
    return ret;
}
