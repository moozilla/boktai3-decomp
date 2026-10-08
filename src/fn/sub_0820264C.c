#include "global.h"
struct S { u32 w0; u8 p[0x2a - 4]; u16 h2a; };
void sub_0820264C(u32 a, u32 b, struct S *p)
{
    p->w0 = p->h2a;
}
