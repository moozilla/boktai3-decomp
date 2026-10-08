#include "global.h"
u16 sub_08244834(u32);
void sub_0824490C(void);
struct T { u8 f[4]; u8 b4; };
extern struct T *gUnk_03006A50;
void sub_08244208(void) {
    u16 r = sub_08244834(0x13);
    if (r == 0) {
        gUnk_03006A50->b4 = r;
        sub_0824490C();
    }
}
