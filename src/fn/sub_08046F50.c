#include "global.h"

struct S { u8 f[5]; u8 a; u8 b; };
void sub_08046F50(struct S *p)
{
    p->b++;
    p->a = 0;
}
