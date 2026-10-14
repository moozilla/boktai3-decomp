#include "global.h"
u8 *Script_GetPc(void);
void sub_0821AAD8(u8 *);
u32 sub_0821B540(u8 *, s32);
/* Read variable descriptors (8 bytes each, element count at +4) from the
 * script and save up to 512 of their values after the count word. */
s32 sub_0822BEE4(u32 *save)
{
    u8 *desc = (u8 *)(save + 1);
    u32 *value = (u32 *)((u8 *)save + 0x1004);
    while (*save <= 0x1FF && Script_GetPc()) {
        s32 i;
        u8 *next;
        sub_0821AAD8(desc);
        i = 0;
        next = desc + 8;
        for (; i < *(u16 *)(desc + 4) && *save <= 0x1FF; i++) {
            *value++ = sub_0821B540(desc, i);
            (*save)++;
        }
        desc = next;
    }
    return 0;
}
