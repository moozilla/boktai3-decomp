#include "global.h"
void sub_08239D38(u8 *);
void sub_08239E6C(u8 *p)
{
    u8 t = p[0x5BB];
    s32 n;
    if (t != 0) {
        n = t;
        do {
            sub_08239D38(p);
            n--;
        } while (n != 0);
    }
}
