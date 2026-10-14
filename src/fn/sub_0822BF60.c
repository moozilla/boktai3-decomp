#include "global.h"
void sub_0821B4C8(u8 *, s32, u32);
/* Write saved values back through the descriptors written by sub_0822BEE4. */
s32 sub_0822BF60(u32 *save)
{
    u8 *desc = (u8 *)(save + 1);
    u32 *value = (u32 *)((u8 *)save + 0x1004);
    u32 n = 0;
    while (n < *save) {
        s32 i = 0;
        u8 *next = desc + 8;
        for (; i < *(u16 *)(desc + 4) && n < *save; i++) {
            sub_0821B4C8(desc, i, *value++);
            n++;
        }
        desc = next;
    }
    return 0;
}
