#include "global.h"
struct S { u8 pad; u8 a; u8 b; u8 g[9]; s8 arr[1]; };
s8 sub_0816B3C0(struct S *p)
{
    return p->arr[p->a * 4 + p->b];
}
