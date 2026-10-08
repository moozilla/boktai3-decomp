#include "global.h"

void sub_08214514(u8 *);

void sub_08098324(u8 *p) {
    u8 *q = p + 0x3d4;
    s32 i = 3;
    do {
        sub_08214514(q);
        q += 0x60;
        i--;
    } while (i >= 0);
}
