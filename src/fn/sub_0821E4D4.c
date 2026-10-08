#include "global.h"
struct E { u16 f0; u16 f2; u8 p[4]; u8 f8; };
struct O { u16 a; u16 b; u16 c; };
struct E *sub_0821E104(u16, u16 *);
void sub_0821E4D4(u16 id, struct O *o)
{
    u16 n;
    struct E *e = sub_0821E104(id, &n);
    if (e) {
        o->a = e->f0;
        o->b = e->f8 << 4;
        o->c = e->f2;
    }
}
