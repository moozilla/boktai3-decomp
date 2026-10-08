#include "global.h"

struct S { u8 f[0x19]; u8 a; u8 g[2]; u32 b; };
void sub_08036F30(struct S *p)
{
    if (p->a) p->a = 0;
    p->b++;
}
