#include "global.h"

struct P { u8 filler[0x30]; u32 a; u32 b; };
struct Q { u32 a; u32 b; };

void sub_0815AED4(struct P *p, struct Q *q)
{
    u32 t = q->b;
    p->a = q->a;
    p->b = t;
}
