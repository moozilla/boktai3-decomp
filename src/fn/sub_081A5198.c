#include "global.h"

void Sprite_Remove(void *);
void sub_08013B74(void *);
void Collider_Remove(void *);
struct A {
    u8 f0[0x18];
    u8 f18[0x48];
    u8 f60[0x6c];
    u8 fcc[1];
};

void sub_081A5198(struct A *p)
{
    Sprite_Remove(p->f18);
    sub_08013B74(p->fcc);
    Collider_Remove(p->f60);
}
