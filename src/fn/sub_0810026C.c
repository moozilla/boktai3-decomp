#include "global.h"

s32 sub_0821ABA8(s32, s32);
u8 *sub_080FF820(void);
void sub_080FF78C(u8 *, s32);
void sub_080FF79C(u8 *, s32);

void sub_0810026C(void)
{
    if (sub_0821ABA8(0x6e, 0) != 0) {
        u8 *p = sub_080FF820();
        if (p != 0) {
            sub_080FF78C(p, 0x18);
            sub_080FF79C(p, 4);
        }
    }
}
