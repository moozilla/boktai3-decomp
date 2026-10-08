#include "global.h"
void sub_081383A4(u8 *p)
{
    u8 *q = p + 0xe0;
    s32 i = 0;
    if (i < *(u16 *)(p + 0x1a)) {
        u8 *t = p + 0x164;
        do {
            t[i] = i;
            i++;
        } while (i < *(u16 *)(p + 0x1a));
    }
    if (i <= 4) {
        u8 *r = q + 0x84;
        u32 four = 4;
        do {
            r[i] = four;
            i++;
        } while (i <= 4);
    }
}
