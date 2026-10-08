#include "global.h"

void sub_080614B4(u8 *, s32);
void sub_0805E7C4(u8 *, s32);
void sub_080613BC(u8 *, s32);

void sub_08061600(u8 *p)
{
    sub_080614B4(p, 1);
    sub_0805E7C4(p + 0x1BE4, 0x1e);
    sub_080613BC(p, 1);
}
