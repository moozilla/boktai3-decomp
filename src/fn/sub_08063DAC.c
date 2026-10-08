#include "global.h"

void sub_0821BC68(u8 *, u8 *);
void sub_08225AF0(u8 *, u8 *, s32, s32);

void sub_08063DAC(u8 *p)
{
    u8 *q = p + 0xac;
    sub_0821BC68(q, p + 0x28);
    sub_08225AF0(p + 0x1c, q, 0x1e, 0x1e);
}
