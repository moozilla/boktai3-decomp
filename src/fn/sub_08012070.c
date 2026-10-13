#include "animated_hazard.h"

void AnimatedHazard_Initialize(struct AnimatedHazardManager *m, struct AnimatedHazard *p)
{
    struct HazardExtent size, offset;
    int zero;
    u8 *sprite = (u8 *)&p->sprite, *effect;
    Sprite_Init(sprite, m->animData, 1);
    zero = 0;
    sprite[7] = 2;
    Anim_SetSequence(&p->anim, m->script, 0, 0, zero);
    effect = p->collider;
    size.x = 64;
    size.y = 128;
    size.z = 64;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(effect, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetCallback(effect, 0, p);
    sub_0821FF58(effect, 100, 50, 0, 0x60000, 30);
    p->active = zero;
}
