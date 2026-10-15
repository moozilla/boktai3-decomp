#include "stacked_actor.h"
void Trigger_ConsumePending(void *, u32);
void StackedActor_RetractToIdle(struct StackedActor *o)
{
    o->sprite.flags &= ~1;
    if (o->sprite.position.y < o->world.y + 22)
        o->sprite.position.y += 2;
    if (o->height != 0) {
        o->height--;
        StackedActor_UpdateSegments(o);
    } else {
        o->shadow.flags &= ~1;
        StackedActor_SetUpdate(o, StackedActor_TickIdle);
        Trigger_ConsumePending(&o->trigger, 1);
    }
}
