#include "global.h"
struct S { u8 f[0xa8]; u16 h; u8 b; };
struct T { u32 fl; u8 p[8]; u16 h; };
void sub_08115A20(void *, void *);
void sub_08115A98(struct S *s, struct T *t)
{
    u32 m = 0x20;
    if (t->fl & m) {
        s->b = 0;
    } else {
        sub_08115A20(s, t);
        s->h = t->h;
    }
}
