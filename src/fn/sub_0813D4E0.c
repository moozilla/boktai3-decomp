#include "global.h"
void sub_0813D3B0(u8 *);
void sub_0813D4E0(u8 *p)
{
    u8 t = p[0x5BB];
    s32 n;
    if (t != 0) {
        n = t;
        do {
            sub_0813D3B0(p);
            n--;
        } while (n != 0);
    }
}
