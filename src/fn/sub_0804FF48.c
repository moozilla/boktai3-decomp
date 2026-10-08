#include "global.h"
void sub_0804FC84(u8 *);
u32 sub_0804FF48(u8 *p)
{
    s32 i = 0;
    if (i < *(u16 *)(p + 0x18)) {
        u8 *q = p + 0x1C;
        do {
            sub_0804FC84(q);
            q += 0xFC;
            i++;
        } while (i < *(u16 *)(p + 0x18));
    }
    return 0;
}
