#include "global.h"

s32 sub_081593D8(u8 *p, s32 mask)
{
    if ((*(u16 *)(p + 0x474) & mask) != 0) {
        return 1;
    }
    return 0;
}
