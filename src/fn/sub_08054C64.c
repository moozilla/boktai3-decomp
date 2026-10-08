#include "global.h"
u8 *sub_08054B80(void);
void sub_08054C64(void)
{
    u8 *p = sub_08054B80();
    if (p) {
        *(u32 *)(p + 0x28) |= 1;
    }
}
