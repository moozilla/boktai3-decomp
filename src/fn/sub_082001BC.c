#include "global.h"
struct S { u8 p[0xc]; u32 w; };
void sub_082001BC(struct S *p, u32 v)
{
    p->w = v;
}
