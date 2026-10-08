#include "global.h"

struct S { u8 f[0x1AA4]; u8 a; u8 g1[2]; u8 m; };

void sub_080614B4(struct S *, s32);
void sub_0805E7C4(u8 *, s32);
void sub_080613BC(struct S *, s32);

void sub_08061BCC(struct S *p)
{
    sub_080614B4(p, 3);
    p->a = 0x80 | p->a;
    sub_0805E7C4((u8 *)p + 0x1BE4, 0x1e);
    sub_080613BC(p, 1);
}
