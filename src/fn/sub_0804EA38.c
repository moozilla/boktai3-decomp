#include "global.h"

struct S { u8 filler[0x1c]; u16 v; };
s32 sub_08228DB8(void);

s32 sub_0804EA38(struct S *p)
{
    if (p->v != 0) {
        if ((u32)(sub_08228DB8() - 1) > 2)
            return 1;
    }
    return 0;
}
