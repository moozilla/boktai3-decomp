#include "global.h"

struct S { u8 f[0x1328]; u16 a; };

void sub_080586B0(struct S *p)
{
    p->a = 0x30;
}
