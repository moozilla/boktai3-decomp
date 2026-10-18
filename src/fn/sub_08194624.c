#include "global.h"

u32 Nidhoggr_IsSuspendedObjectReady(u8 *p)
{
    u32 r = 0;
    if (*(p + 0x71BC) == 4) {
        r = 1;
    }
    return r;
}
