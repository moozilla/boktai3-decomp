#include "global.h"

struct S {
    u8 f[0x12c];
    u8 a;
};
void Actor_ClearStateChanged(struct S *p)
{
    if (p->a)
        p->a = 0;
}
