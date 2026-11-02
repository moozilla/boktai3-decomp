#include "vanargand_effect.h"
void VanargandEffect_Update(struct VanargandEffect *p)
{
    u16 *c = &p->delay;
    if (*c) {
        (*c)--;
        if (*c == 0)
            p->sprite.flags &= ~1;
    } else {
        ActorEffect_UpdateFadeFrame(p);
        VanargandEffect_UpdateLifetime(p);
    }
}
