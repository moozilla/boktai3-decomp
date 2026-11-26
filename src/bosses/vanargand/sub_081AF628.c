#include "global.h"

struct A {
    u8 f[0x18];
    u8 g[4];
    u8 a;
};
void Sprite_Remove(void *);

void Vanargand_DestroyTransientEffect(struct A *p)
{
    if (p->a != 0)
        Sprite_Remove(p->g);
}
