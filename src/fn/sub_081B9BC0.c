#include "global.h"
void sub_08221110(void *, void *, void *, int, int);
void sub_081B9BC0(u8 *a, u8 *b, u8 *c, int d, int e, int n)
{
    u8 *alias;
    if (n > 0) {
        u8 *z = c;
        u8 *y = b;
        u8 *x = a;
        int count = n;
        /* do-while(0) only keeps agbcc's block layout */
        do {
            alias = z;
            sub_08221110(x, y, alias, d, e);
            z += 0x20;
            y += 0x20;
            /* do-while(0) only keeps agbcc's block layout */
            do {
                x += (unsigned short)0x20;
            } while (0);
        } while (--count);
    }
}
