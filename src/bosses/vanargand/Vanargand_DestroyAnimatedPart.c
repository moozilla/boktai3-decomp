#include "global.h"

struct A {
    u8 f[0xa8];
    u8 g[4];
    u8 a;
};
void Anim_Remove(void *);
void Sprite_Remove(void *);

void Vanargand_DestroyAnimatedPart(struct A *p)
{
    Anim_Remove(p);
    if (p->a != 0)
        Sprite_Remove(p->g);
}
