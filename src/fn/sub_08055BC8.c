#include "global.h"

struct S {
    u8 f[0x1cc];
    u32 v;
};
void Script_Call(u32, u32);

void sub_08055BC8(struct S *p)
{
    if (p->v)
        Script_Call(p->v, 0);
}
