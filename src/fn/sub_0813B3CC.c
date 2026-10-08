#include "global.h"

struct S0813B3CC { u8 filler[0xB38]; void *v; };
void sub_0821AD08(void *, u32);

void sub_0813B3CC(struct S0813B3CC *p)
{
    if (p->v != 0)
        sub_0821AD08(p->v, 0);
}
