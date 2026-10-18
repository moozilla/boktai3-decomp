#include "global.h"

struct S {
    u8 f[0x888];
    u8 a;
};
void Sprite_Remove(void *);
void GridAttackActor_RemoveSprite(struct S *p)
{
    Sprite_Remove(&p->a);
}
