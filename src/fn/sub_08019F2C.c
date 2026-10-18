#include "global.h"
u32 SelectionMenu_ConsumeCallbackFlag(u8 *p)
{
    u32 r;
    if (p[0x1c] == 0) {
        r = 0;
    } else {
        p[0x1c] = 0;
        r = 1;
    }
    return r;
}
