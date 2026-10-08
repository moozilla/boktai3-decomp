#include "global.h"

void sub_081DAD28(s32);

void sub_0814F310(u8 *p)
{
    s32 v = *(s32 *)(p + 0xC74);

    if (v >= 0) {
        sub_081DAD28(v);
    }
}
