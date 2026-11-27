#include "global.h"

struct S {
    u8 f[0x110];
    u32 a[1];
};
u32 TextState_SetString(struct S *p, u32 i, u32 v)
{
    p->a[i] = v;
}
