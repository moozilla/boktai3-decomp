#include "global.h"
void sub_08013424(void *);
void sub_0801370C(void *);
u32 sub_0802D8BC(u8 *p) {
    u8 v = p[0x218];
    if (v == 1) sub_08013424(p + 0x244);
    else if (v == 2) sub_0801370C(p + 0x244);
    return 0;
}
