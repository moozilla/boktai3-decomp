#include "global.h"

void sub_0813D150(u8 *p)
{
    if (*(s32 *)(p + 0x18) == 0) {
        *(u16 *)(p + 0xAC2) = 0x2A;
    } else {
        *(u16 *)(p + 0xAC2) = 0x35;
    }
}
