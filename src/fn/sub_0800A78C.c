#include "global.h"
extern u8 *gUnk_0200003C;
void sub_0821AAD8(void *);
void sub_0821B4C8(void *, s32, s32);
s32 sub_0800A78C(void) {
    s32 count;
    u8 *base;
    s32 i;
    u8 *item;
    base = gUnk_0200003C;
    count = 0;
    if (base != 0) {
        item = base + 0x2C;
        for (i = 0xF; i >= 0; i--) {
            if (item[5] != 0)
                count++;
            item += 0xA0;
        }
    }
    {
        u8 temp[8];
        sub_0821AAD8(temp);
        sub_0821B4C8(temp, 0, count);
    }
    return count;
}
