#include "global.h"
struct E { u8 f[0x28]; };
struct Q { u8 f[0x12]; u8 a; s8 idx; u8 g[4]; u32 b; };
void sub_08217EAC(void *);
void sub_08219DD8(void *, u32);
void sub_08114E04(u8 *p, struct Q *q)
{
    sub_08217EAC(p + 0x20 + q->idx * 0x28);
    q->a = 0;
    q->b = 0;
    sub_08219DD8(q, 0x1c);
    q->idx = -1;
}
