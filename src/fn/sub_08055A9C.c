#include "global.h"
s32 sub_08055828(u8 *);
s32 sub_08055A9C(u8 *p)
{
    s32 i = 0;
    if (i < p[0x19]) {
        u8 *q = p + 0x1c;
        do {
            sub_08055828(q);
            q += 0xbc;
            i++;
        } while (i < p[0x19]);
    }
    return 0;
}
