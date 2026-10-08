#include "global.h"

s32 sub_0821ABA8(s32, s32);
s32 sub_080FF820(void);
void sub_080FF79C(s32, s32);

void sub_0810031C(void)
{
    if (sub_0821ABA8(0x6e, 0) != 0) {
        s32 r = sub_080FF820();
        if (r != 0)
            sub_080FF79C(r, 0x10);
    }
}
