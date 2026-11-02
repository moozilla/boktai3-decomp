#include "vanargand_effect.h"

void Sprite_Remove(void *);

void VanargandEffect_Destroy(struct VanargandEffect *p)
{
    if (p->sprite.linked != 0)
        Sprite_Remove(&p->sprite);
}
