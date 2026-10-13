#include "global.h"

struct S {
    u8 f[0x20];
    void *p20;
    void *p24;
};
extern u32 gUnk_0200046C;
void Sprite_Remove(void *);
void sub_0800CC08(u32, void *);
void sub_0800CC34(void *);
void Collider_Remove(void *);
void Heap_Free(void *);

u32 sub_0800CC50(struct S *s)
{
    Sprite_Remove((u8 *)s + 0x44);
    if (gUnk_0200046C != 0)
        sub_0800CC08(gUnk_0200046C, s);
    sub_0800CC34(s);
    if (s->p20 != 0) {
        Collider_Remove(s->p20);
        Heap_Free(s->p20);
    }
    if (s->p24 != 0)
        Heap_Free(s->p24);
    Heap_Free(s);
}
