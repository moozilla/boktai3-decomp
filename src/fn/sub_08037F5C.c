#include "global.h"
void sub_0803A618(u32, u32, u32);
void sub_08037F5C(u8 *p)
{
    u8 *q = p + 0x3c;
    u8 *b = p + 0x48;
    s32 i;
    u32 z;
    if (q[6] != 0) {
        u8 *c;
        u8 *a;
        i = 0;
        z = 0;
        c = b;
        a = b - 0x10;
        do {
            *a += *c;
            sub_0803A618(i, *c, 1);
            *c = z;
            c++;
            a++;
            i++;
        } while (i <= 3);
    } else {
        i = 0;
        z = 0;
        do {
            u8 *c = b + i;
            sub_0803A618(i, *c, 0);
            *c = z;
            i++;
        } while (i <= 3);
    }
}
