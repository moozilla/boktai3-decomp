#include "global.h"

struct S { u8 f[0x36]; u8 a, b, c, d; u8 g[0x22]; u32 x; u32 y; };
void sub_08030F20(u32, u32, u32, u32);
void sub_08030B78(u32);
void sub_08030C84(u32);
void sub_08030DBC(u32);

void sub_08006738(struct S *s)
{
    sub_08030F20(s->a, s->b, s->c, s->d);
    sub_08030B78(1);
    sub_08030C84(s->x);
    sub_08030DBC(s->y);
}
