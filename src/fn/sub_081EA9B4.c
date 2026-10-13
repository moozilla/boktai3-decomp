#include "global.h"
/* Hide all nine animated display objects. */
void sub_081EA9B4(u8 *p)
{
    *(u32 *)(p + 0x20) |= 1;
    *(u32 *)(p + 0x350) |= 1;
    *(u32 *)(p + 0x3d8) |= 1;
    *(u32 *)(p + 0xa8) |= 1;
    *(u32 *)(p + 0x1b8) |= 1;
    *(u32 *)(p + 0x130) |= 1;
    *(u32 *)(p + 0x240) |= 1;
    *(u32 *)(p + 0x2c8) |= 1;
    *(u32 *)(p + 0x460) |= 1;
}
