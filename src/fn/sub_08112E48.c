#include "global.h"
struct A { u8 f[0x20]; u8 g[4]; u8 on; };
struct S { struct A *a; u8 f[0x15]; u8 x; };
void sub_082195E0(void *);
void sub_08219D38(void *);
void sub_08112E48(struct S *s)
{
    struct A *a = s->a;
    if (a->on != 0) {
        sub_082195E0(a->g);
        sub_08219D38(s->a);
        s->x = 0;
    }
}
