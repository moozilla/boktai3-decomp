#include "global.h"
void sub_08013684(void *);
void sub_08013B74(void *);
u32 sub_0802D85C(u8 *p) {
    u8 v = p[0x218];
    if (v == 1) sub_08013684(p + 0x244);
    else if (v == 2) sub_08013B74(p + 0x244);
    return 0;
}
