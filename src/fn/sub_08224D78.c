#include "global.h"

u32 rfu_REQBN_softReset_and_checkID(void);

s32 sub_08224D78(void)
{
    if (rfu_REQBN_softReset_and_checkID() == 0x8001)
        return 1;
    if (*(u32 *)0x03004264 > 0x1C1)
        return -1;
    return 0;
}
