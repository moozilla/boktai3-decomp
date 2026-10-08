#include "global.h"

struct S { u8 f[0x29]; u8 a; };
void sub_08038674(struct S *p)
{
    if (p->a) p->a = 0;
}
