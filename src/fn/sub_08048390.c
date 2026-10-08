#include "global.h"

struct S { u8 f[0x18]; u8 a; };
u32 sub_08048390(struct S *p)
{
    if (!p)
        return 0;
    return p->a;
}
