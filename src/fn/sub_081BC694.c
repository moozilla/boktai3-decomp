#include "global.h"
struct P { u8 f[0x904]; u16 b; u8 g[0x930-0x906]; u32 a; };
void sub_081BC694(struct P *p, u32 v)
{
    p->a = v;
    p->b = 0;
}
