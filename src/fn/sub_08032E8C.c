#include "global.h"

struct S { u8 f[0x150]; u32 a; };
void sub_08032E8C(struct S *p)
{
    p->a = 0;
}
