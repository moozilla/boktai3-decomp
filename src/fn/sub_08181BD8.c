#include "global.h"
struct Sub { void (*fn)(void); u32 z; };
struct S { u8 f[0x650]; struct Sub s; u8 g[0xAD8-0x658]; u16 h; };
void sub_08181B00(void);
void sub_08181BD8(struct S *p)
{
    struct Sub *s = &p->s;
    void (*f)(void) = sub_08181B00;
    u32 z = 0;
    s->fn = f;
    s->z = z;
    p->h = z;
}
