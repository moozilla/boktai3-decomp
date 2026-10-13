#include "global.h"
struct A {
    u8 f[0x1c];
    u8 g[4];
    u8 on;
};
struct S {
    struct A *a;
    u8 f[0x15];
    u8 x;
};
void Sprite_Remove(void *);
void Heap_Free(void *);
void sub_08112E20(struct S *s)
{
    struct A *a = s->a;
    if (a->on != 0) {
        Sprite_Remove(a->g);
        Heap_Free(s->a);
        s->x = 0;
    }
}
