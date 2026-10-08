#include "global.h"

struct S { u8 f[5]; u8 v; };
void sub_08217EAC(void *);
void sub_0821FE6C(void *);

u32 sub_08009F34(u32 a, struct S *s)
{
    sub_08217EAC((u8 *)s + 0x68);
    sub_0821FE6C((u8 *)s + 0x14);
    return s->v = 0;
}
