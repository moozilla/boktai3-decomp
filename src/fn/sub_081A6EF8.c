#include "global.h"
void sub_0821FF8C(void *);
void sub_08159860(void *, void *, u32);
struct Dest { u8 pad[0x124]; u32 a; u16 b; u8 pad2[4]; u16 c; };
void sub_081A6EF8(u8 *a, u8 *b, struct Dest *c)
{
    u32 v;
    sub_0821FF8C(a);
    c->b = *(u16 *)(b + 0x3e);
    v = *(u32 *)(a + 0x38);
    c->a = v;
    c->c = *(u16 *)(a + 0x44);
    sub_08159860(a, b, 10);
}
