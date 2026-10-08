#include "global.h"
struct P { u8 f[0x70C]; u16 b; u8 g[0x730-0x70E]; u32 a; };
void sub_081BB520(struct P *p, u32 v)
{
    p->a = v;
    p->b = 0;
}
