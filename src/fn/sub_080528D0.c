#include "stacked_actor.h"
#define StackedActor_RetractToIdle sub_080528D0
void Trigger_ConsumePending(void *, u32);
void StackedActor_RetractToIdle(struct StackedActor *o)
{
    o->sprite.flags &= ~1;
    if (o->sprite.position.y < o->world.y + 22)
        o->sprite.position.y += 2;
    if (o->height != 0) {
        o->height--;
        sub_08052418(o);
    } else {
        o->shadow.flags &= ~1;
        sub_08052628(o, sub_08052A40);
        Trigger_ConsumePending(&o->trigger, 1);
    }
}
