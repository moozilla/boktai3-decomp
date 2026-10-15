#include "global.h"

struct S {
    u8 f[0x1AA4];
    u8 a;
    u8 g1[2];
    u8 m;
};

void sub_080614B4(struct S *, s32);
void SceneInput_SetGridFlags(u8 *, s32);
void sub_080613BC(struct S *, s32);

void sub_08061C04(struct S *p)
{
    sub_080614B4(p, 2);
    p->a = 0x80 | p->a;
    SceneInput_SetGridFlags((u8 *)p + 0x1BE4, 0x1e);
    sub_080613BC(p, 1);
}
