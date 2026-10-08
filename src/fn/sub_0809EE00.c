#include "global.h"

void sub_08076034(u8 *, u32);
void sub_080A0874(u8 *);

void sub_0809EE00(u8 *p) {
    if (p[0x2b2] != 0) p[0x2b2] = 0;
    sub_08076034(p, 1);
    sub_080A0874(p);
}
