#include "global.h"

extern u8 *gUnk_020001A8;

s32 sub_081009F4(u8 *obj)
{
    gUnk_020001A8 = obj;
    *(u32 *)(obj + 0x498) = 0;
    return 0;
}
