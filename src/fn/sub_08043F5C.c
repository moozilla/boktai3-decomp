#include "global.h"

struct S { u8 f[0x24]; u32 a; };
void sub_08043F5C(struct S *p)
{
    p->a = 600;
}
