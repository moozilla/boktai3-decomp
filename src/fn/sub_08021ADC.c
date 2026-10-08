#include "global.h"

struct S { u8 f[0xe8]; u32 a; u32 b; };
void sub_08021ADC(struct S *p, u32 a, u32 b)
{
    p->a = a;
    p->b = b;
}
