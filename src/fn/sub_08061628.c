#include "global.h"

struct D { u32 a, b; };
struct S { u8 f[0xD88]; struct D d1; u8 g[0xDE8-0xD90]; struct D d2; u8 h[0x1AA4-0xDF0]; u8 a; };

void sub_0805EA00(u8 *);
void sub_0805E7C4(u8 *, s32);
void sub_080613BC(struct S *, s32);
void sub_0806130C(struct S *, void (*)(void));
void sub_08062190(void);

void sub_08061628(struct S *p)
{
    u8 *q = (u8 *)p + 0x1BE4;
    sub_0805EA00(q);
    p->a = 0x2c;
    p->d2 = p->d1;
    sub_0805E7C4(q, 0x1e);
    sub_080613BC(p, 1);
    sub_0806130C(p, sub_08062190);
}
