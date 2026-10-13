#include "global.h"
struct A {
    u8 f[0x20];
    u8 g[4];
    u8 on;
};
struct S {
    struct A *a;
    u8 f[0x15];
    u8 x;
};
void Anim_Remove(void *);
void Heap_Free(void *);
void sub_08112E48(struct S *s)
{
    struct A *a = s->a;
    if (a->on != 0) {
        Anim_Remove(a->g);
        Heap_Free(s->a);
        s->x = 0;
    }
}
