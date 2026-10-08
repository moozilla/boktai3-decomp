#include "global.h"
void sub_08217EAC(u8 *);
u32 sub_08017700(u8 *p)
{
    u8 *e = p + 0x16c;
    s32 i = 0x1f;
    u8 *q;
    do {
        q = e + 0x14;
        if (*e != 0) {
            sub_08217EAC(q);
            *e = 0;
        }
        i--;
        e += 0x3c;
    } while (i >= 0);
}
