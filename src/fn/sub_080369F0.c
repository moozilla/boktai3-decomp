#include "global.h"

extern u8 *gUnk_02000484;

s32 sub_080369F0(void)
{
    if (gUnk_02000484 == NULL)
        return -1;
    return (s8)gUnk_02000484[0x1f];
}
