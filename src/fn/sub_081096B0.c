#include "global.h"

s32 sub_081096B0(void *p)
{
    u32 v = *(u32 *)((u8 *)p + 0x38);
    switch (v) {
    case 3:
        return 0;
    case 0x10:
        return 1;
    default:
        return 2;
    }
}
