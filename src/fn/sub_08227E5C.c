#include "global.h"
extern u8 *gUnk_02000710;
void sub_08227E5C(s32 index)
{
    u32 base = (u32)gUnk_02000710;
    s16 *counter = (s16 *)((index * 2) + base + 0x884);
    u16 v = *counter;
    if (*counter <= 9998)
        *counter = v + 1;
}
