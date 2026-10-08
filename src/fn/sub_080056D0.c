#include "global.h"

struct S { u8 f[0x70]; u32 a; };
extern u32 gUnk_02000024;
void sub_080047EC(struct S *);
void sub_08219D38(u32);
u32 sub_080056D0(struct S *p)
{
    sub_080047EC(p);
    sub_08219D38(p->a);
    return gUnk_02000024 = 0;
}
