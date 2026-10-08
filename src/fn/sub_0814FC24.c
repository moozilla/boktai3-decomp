#include "global.h"

s32 sub_0813AE18(u8 *, s32, s32, s32);
void sub_0813ADD0(u8 *);
void sub_0814F804(u8 *);
void sub_0814F89C(u8 *);

void sub_0814FC24(u8 *p)
{
    if (sub_0813AE18(p, 0x161, 0x40, 1) != 0) {
        sub_0813ADD0(p);
        sub_0814F804(p);
        sub_0814F89C(p);
    }
}
