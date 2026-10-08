#include "global.h"

struct S { u8 f[0x28]; u16 fl; };
u32 sub_08228D7C(void);
void sub_082286E0(s32 *, s32 *, s32 *, u32);
void sub_0800F4EC(struct S *);

void sub_0800F6B0(struct S *s)
{
    s32 a, b, c;
    u32 z;
    sub_082286E0(&a, &b, &c, sub_08228D7C());
    z = 0;
    s->fl = 0xf;
    sub_0800F4EC(s);
    s->fl = z;
}
