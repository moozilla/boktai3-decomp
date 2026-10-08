#include "global.h"
void sub_08214514(u8 *);
void sub_0805AD18(u8 *p)
{
    u8 *q;
    s32 j;
    sub_08214514(p);
    q = p + 0x68;
    j = 7;
    do {
        sub_08214514(q);
        q += 0x68;
        j--;
    } while (j >= 0);
    q = p + 0x3a8;
    j = 5;
    do {
        sub_08214514(q);
        q += 0x68;
        j--;
    } while (j >= 0);
}
