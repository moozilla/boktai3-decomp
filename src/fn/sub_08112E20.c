#include "global.h"
struct A { u8 f[0x1c]; u8 g[4]; u8 on; };
struct S { struct A *a; u8 f[0x15]; u8 x; };
void sub_08214514(void *);
void sub_08219D38(void *);
void sub_08112E20(struct S *s)
{
    struct A *a = s->a;
    if (a->on != 0) {
        sub_08214514(a->g);
        sub_08219D38(s->a);
        s->x = 0;
    }
}
