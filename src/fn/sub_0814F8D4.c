#include "global.h"

void sub_0813B8A4(u8 *);

void sub_0814F8D4(u8 *p)
{
    u8 *c;
    s32 v;

    if (p[0x5A1] != 0) {
        c = p + 0x5A2;
        v = *c + 1;
        *c = v;
        if ((u8)v > 7) {
            sub_0813B8A4(p);
            *c = 0;
        }
    }
}
