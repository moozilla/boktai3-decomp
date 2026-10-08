#include "global.h"
struct E { u8 f[0x36]; u8 a; u8 b; u8 pad[4]; u8 c; };
struct S { u8 f[0x18]; u32 p; };
void sub_08217EEC(void *, u32, u32);
void sub_08125BBC(struct S *s, struct E *e)
{
    if (e->a != 0) {
        e->a--;
    } else {
        e->a = 3;
        e->b = (e->b + 1) & 1;
        sub_08217EEC(e, s->p, e->c + e->b);
    }
}
