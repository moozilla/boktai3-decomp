#include "global.h"

void sub_08231568(u8 *);
void sub_0813D3B0(u8 *);

void sub_0813D508(u8 *p)
{
    if (p[0x5B9] == 1) {
        sub_08231568(p + 0x5B8);
    } else {
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
}
