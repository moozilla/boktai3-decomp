#include "global.h"

struct S0813B3CC {
    u8 filler[0xB38];
    void *v;
};
void Script_Call(void *, u32);

void sub_0813B3CC(struct S0813B3CC *p)
{
    if (p->v != 0)
        Script_Call(p->v, 0);
}
